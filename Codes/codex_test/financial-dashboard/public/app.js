const $ = selector => document.querySelector(selector);
const money = value => new Intl.NumberFormat('en-GB', { style: 'currency', currency: 'GBP' }).format(value);
const shortMoney = value => new Intl.NumberFormat('en-GB', {
  style: 'currency',
  currency: 'GBP',
  notation: Math.abs(value) >= 10000 ? 'compact' : 'standard',
  maximumFractionDigits: Math.abs(value) >= 1000 ? 1 : 0
}).format(value);
const esc = value => String(value).replace(/[&<>"']/g, char => ({ '&':'&amp;', '<':'&lt;', '>':'&gt;', '"':'&quot;', "'":'&#39;' }[char]));
let allTransactions = [];
let accounts = [];
let balanceSnapshots = {};
let categoryOptions = [];
let categoryExpanded = false;
let collapsedMonths = new Set();
let monthCollapseInitialised = false;

const viewCopy = {
  overview: {
    eyebrow: 'MULTI-ACCOUNT OVERVIEW',
    title: 'Your financial overview.',
    description: 'Internal transfers are matched and excluded from income and spending.'
  },
  transactions: {
    eyebrow: 'TRANSACTION ACTIVITY',
    title: 'Your transactions.',
    description: 'Browse, filter, and recategorise every imported transaction.'
  }
};

function setView(view) {
  const selected = viewCopy[view] ? view : 'overview';
  $('#overviewView').hidden = selected !== 'overview';
  $('#transactionsView').hidden = selected !== 'transactions';
  $('#balanceButton').hidden = selected !== 'overview';
  $('#periodFilter').hidden = false;
  $('#pageEyebrow').textContent = viewCopy[selected].eyebrow;
  $('#pageTitle').textContent = viewCopy[selected].title;
  $('#pageDescription').textContent = viewCopy[selected].description;
  document.querySelectorAll('[data-view-target]').forEach(button => {
    const active = button.dataset.viewTarget === selected;
    button.classList.toggle('active', active);
    button.setAttribute('aria-current', active ? 'page' : 'false');
  });
  document.title = `${selected[0].toUpperCase()}${selected.slice(1)} · Personal Financial Dashboard`;
}

function viewFromHash() {
  const view = window.location.hash.slice(1);
  if (view === 'analytics') {
    window.__openAnalyticsWidgetOnLoad = true;
    window.history.replaceState(null, '', '#overview');
    window.dispatchEvent(new CustomEvent('analytics:open-request'));
    return 'overview';
  }
  return viewCopy[view] ? view : 'overview';
}

async function request(url, options = {}) {
  const response = await fetch(url, {
    headers: options.body ? { 'content-type': 'application/json' } : {},
    ...options
  });
  const data = await response.json();
  if (!response.ok) throw new Error(data.error || 'Request failed');
  return data;
}

async function loadDashboard() {
  const dataset = $('#datasetFilter').value;
  window.__dashboardDataReady = false;
  window.dispatchEvent(new CustomEvent('dashboard:loading', { detail: { dataset } }));
  $('#datasetFilter').disabled = true;
  $('#transactionRows').innerHTML = '<tr><td colspan="6">Reading CSV files…</td></tr>';
  try {
    const data = await request(`/api/transactions?dataset=${encodeURIComponent(dataset)}`);
    allTransactions = data.transactions;
    accounts = data.accounts;
    balanceSnapshots = data.balances || {};
    categoryOptions = data.category_options || [];
    collapsedMonths = new Set();
    monthCollapseInitialised = false;
    setupAccountFilter();
    setupPeriodFilter();
    window.__dashboardDataReady = true;
    window.dispatchEvent(new CustomEvent('dashboard:data-loaded', { detail: { dataset } }));
    $('#fileCount').textContent = `${data.file_count} CSV file${data.file_count === 1 ? '' : 's'} · ${accounts.length} account${accounts.length === 1 ? '' : 's'}`;
    render();
  } catch (error) {
    allTransactions = [];
    window.__dashboardDataReady = false;
    $('#transactionRows').innerHTML = `<tr><td colspan="6">${esc(error.message)}</td></tr>`;
    window.dispatchEvent(new CustomEvent('dashboard:data-failed', { detail: { dataset } }));
  } finally {
    $('#datasetFilter').disabled = false;
  }
}

function setupAccountFilter() {
  $('#accountFilter').innerHTML = '<option value="all">All accounts</option>';
  accounts.forEach(account => {
    $('#accountFilter').insertAdjacentHTML('beforeend', `<option value="${esc(account.key)}">${esc(account.label)}</option>`);
  });
}

function setupPeriodFilter() {
  const select = $('#periodFilter');
  const selected = select.value || 'all';
  const months = [...new Set(allTransactions.map(item => item.date.slice(0, 7)))]
    .filter(Boolean)
    .sort()
    .reverse();
  select.innerHTML = [
    '<option value="all">All time</option>',
    '<option value="30">Last 30 days</option>',
    '<option value="90">Last 90 days</option>',
    ...months.map(key => `<option value="month:${esc(key)}">${esc(monthShortLabel(key))}</option>`)
  ].join('');
  select.value = [...select.options].some(option => option.value === selected) ? selected : 'all';
}

function monthShortLabel(key) {
  return new Date(`${key}-02T12:00:00`).toLocaleDateString('en-GB', { month: 'long', year: 'numeric' });
}

function filtered() {
  const account = $('#accountFilter').value;
  const period = $('#periodFilter').value;
  const days = Number(period);
  const accountItems = allTransactions.filter(item => account === 'all' || item.account_key === account);
  const newest = accountItems.length ? Math.max(...accountItems.map(item => new Date(item.date))) : 0;
  return accountItems.filter(item =>
    (!period.startsWith('month:') || item.date.slice(0, 7) === period.slice(6)) &&
    (!days || (newest - new Date(item.date)) / 86400000 <= days)
  );
}

function internalTransferSummary(items) {
  const pairs = new Map();
  items.filter(item => item.transfer_type === 'internal').forEach(item => {
    const current = pairs.get(item.transfer_pair_id) || 0;
    pairs.set(item.transfer_pair_id, Math.max(current, Math.abs(item.amount)));
  });
  return { count: pairs.size, value: [...pairs.values()].reduce((sum, value) => sum + value, 0) };
}

function externalSlices() {
  const items = filtered();
  const external = items.filter(item => !item.excluded_from_totals);
  return {
    items,
    external,
    incoming: external.filter(item => item.amount > 0),
    outgoing: external.filter(item => item.amount < 0),
  };
}

function render() {
  const { items, external, incoming, outgoing } = externalSlices();
  const income = incoming.reduce((sum, item) => sum + item.amount, 0);
  const spent = -outgoing.reduce((sum, item) => sum + item.amount, 0);

  $('#balance').textContent = money(income - spent);
  $('#income').textContent = money(income);
  $('#spent').textContent = money(spent);
  $('#incomeCount').textContent = `${incoming.length} external incoming payment${incoming.length === 1 ? '' : 's'}`;
  $('#spendCount').textContent = `${outgoing.length} external outgoing payment${outgoing.length === 1 ? '' : 's'}`;

  renderPortfolio();
  renderChart(items, external);
  renderCategories(items, external, outgoing);
  renderRows(items);
}

function metricDetailRows(type) {
  const { external, incoming, outgoing } = externalSlices();
  if (type === 'income') return incoming;
  if (type === 'spending') return outgoing;
  return external;
}

function metricDetailMeta(type, rows) {
  const income = rows.filter(item => item.amount > 0).reduce((sum, item) => sum + item.amount, 0);
  const spending = -rows.filter(item => item.amount < 0).reduce((sum, item) => sum + item.amount, 0);
  if (type === 'income') {
    return {
      eyebrow: 'EXTERNAL INCOME',
      title: 'Income transactions',
      summary: `${rows.length} incoming payment${rows.length === 1 ? '' : 's'} · ${money(income)} total`,
    };
  }
  if (type === 'spending') {
    return {
      eyebrow: 'EXTERNAL SPENDING',
      title: 'Spending transactions',
      summary: `${rows.length} outgoing payment${rows.length === 1 ? '' : 's'} · ${money(spending)} total`,
    };
  }
  return {
    eyebrow: 'EXTERNAL NET FLOW',
    title: 'Income and spending transactions',
    summary: `${rows.length} external transaction${rows.length === 1 ? '' : 's'} · ${money(income - spending)} net`,
  };
}

function openMetricDialog(type) {
  const rows = metricDetailRows(type);
  const meta = metricDetailMeta(type, rows);
  $('#metricEyebrow').textContent = meta.eyebrow;
  $('#metricTitle').textContent = meta.title;
  $('#metricSummary').textContent = meta.summary;
  $('#metricRows').innerHTML = rows.map(item => {
    const [label, badgeClass] = typeLabel(item);
    const description = item.memo || 'Bank transaction';
    return `<div class="metric-detail-row">
      <div><strong>${esc(description)}</strong><small>${new Date(`${item.date}T12:00:00`).toLocaleDateString('en-GB', { day:'2-digit', month:'short', year:'numeric' })} · ${esc(item.account_label)} · ${esc(item.category)}</small></div>
      <span class="type-badge ${badgeClass}">${label}</span>
      <strong class="${item.amount >= 0 ? 'positive' : 'negative'}">${item.amount >= 0 ? '+' : ''}${money(item.amount)}</strong>
    </div>`;
  }).join('') || '<p class="empty-detail">No matching external transactions.</p>';
  $('#metricDialog').showModal();
}

function estimatedPosition(account) {
  const snapshot = balanceSnapshots[account.key];
  if (!snapshot) return null;
  const afterSnapshot = item => item.date > snapshot.as_of;
  if (account.type === 'credit_card') {
    const cardActivity = allTransactions
      .filter(item => item.account_key === account.key && afterSnapshot(item))
      .reduce((sum, item) => sum + item.amount, 0);
    const repayments = allTransactions
      .filter(item => item.linked_account_key === account.key && item.balance_affects_linked_account !== false && afterSnapshot(item))
      .reduce((sum, item) => sum + item.amount, 0);
    const current = snapshot.balance - cardActivity + repayments;
    return { current, change: current - snapshot.balance, snapshot };
  }
  const movement = allTransactions
    .filter(item => item.account_key === account.key && afterSnapshot(item))
    .reduce((sum, item) => sum + item.amount, 0);
  return { current: snapshot.balance + movement, change: movement, snapshot };
}

function renderPortfolio() {
  const positions = accounts.map(account => ({ account, position: estimatedPosition(account) }));
  const configured = positions.filter(item => item.position);
  const deposits = configured
    .filter(item => item.account.type !== 'credit_card')
    .reduce((sum, item) => sum + item.position.current, 0);
  const debt = configured
    .filter(item => item.account.type === 'credit_card')
    .reduce((sum, item) => sum + item.position.current, 0);

  $('#totalDeposits').textContent = configured.some(item => item.account.type !== 'credit_card') ? money(deposits) : '—';
  $('#totalDebt').textContent = configured.some(item => item.account.type === 'credit_card') ? money(debt) : '—';
  $('#netPosition').textContent = configured.length ? money(deposits - debt) : '—';
  $('#portfolioStatus').textContent = configured.length
    ? `${configured.length} of ${accounts.length} accounts configured`
    : 'Add balances to track your position';

  $('#accountPositions').innerHTML = positions.map(({ account, position }) => {
    if (!position) return `<button class="position-row unset-position" data-open-balances="true"><span>${esc(account.label)}<small>No balance snapshot</small></span><strong>Set balance →</strong></button>`;
    const changeClass = position.change > 0 ? 'positive' : position.change < 0 ? 'negative' : '';
    const valueLabel = account.type === 'credit_card' ? 'estimated debt' : 'estimated balance';
    return `<div class="position-row"><span>${esc(account.label)}<small>${valueLabel} · snapshot ${esc(position.snapshot.as_of)}</small></span><strong>${money(position.current)}<small class="${changeClass}">${position.change >= 0 ? '+' : ''}${money(position.change)} since snapshot</small></strong></div>`;
  }).join('');
  document.querySelectorAll('[data-open-balances]').forEach(button => button.onclick = openBalanceDialog);
}

function accountBalanceAt(account, targetDate) {
  const snapshot = balanceSnapshots[account.key];
  if (!snapshot || account.type === 'credit_card') return null;
  const accountItems = allTransactions.filter(item => item.account_key === account.key);
  if (targetDate >= snapshot.as_of) {
    const movement = accountItems
      .filter(item => item.date > snapshot.as_of && item.date <= targetDate)
      .reduce((sum, item) => sum + item.amount, 0);
    return snapshot.balance + movement;
  }
  const movementAfterTarget = accountItems
    .filter(item => item.date > targetDate && item.date <= snapshot.as_of)
    .reduce((sum, item) => sum + item.amount, 0);
  return snapshot.balance - movementAfterTarget;
}

function monthEnd(key, latestDate) {
  if (latestDate && latestDate.slice(0, 7) === key) return latestDate;
  const [year, month] = key.split('-').map(Number);
  return new Date(Date.UTC(year, month, 0)).toISOString().slice(0, 10);
}

function openBalanceDialog() {
  const latestDate = allTransactions.reduce((latest, item) => item.date > latest ? item.date : latest, '');
  $('#balanceFields').innerHTML = accounts.map(account => {
    const snapshot = balanceSnapshots[account.key] || {};
    const isCard = account.type === 'credit_card';
    return `<div class="balance-field" data-account-key="${esc(account.key)}">
      <div><strong>${esc(account.label)}</strong><small>${isCard ? 'Enter outstanding debt as a positive number' : 'Enter the end-of-day account balance'}</small></div>
      <label>${isCard ? 'Debt' : 'Balance'}<input class="snapshot-balance" type="number" step="0.01" ${isCard ? 'min="0"' : ''} value="${snapshot.balance ?? ''}" placeholder="0.00"></label>
      <label>As of date<input class="snapshot-date" type="date" value="${snapshot.as_of || latestDate}"></label>
    </div>`;
  }).join('');
  $('#balanceError').textContent = '';
  $('#balanceDialog').showModal();
}

async function saveBalances(event) {
  event.preventDefault();
  const balances = [];
  for (const row of document.querySelectorAll('.balance-field')) {
    const balance = row.querySelector('.snapshot-balance').value;
    const asOf = row.querySelector('.snapshot-date').value;
    if (balance === '') continue;
    if (!asOf) {
      $('#balanceError').textContent = 'Enter both a balance and date, or leave both blank.';
      return;
    }
    balances.push({ account_key: row.dataset.accountKey, balance: Number(balance), as_of: asOf });
  }
  try {
    const data = await request('/api/balances', {
      method: 'POST',
      body: JSON.stringify({ dataset: $('#datasetFilter').value, balances })
    });
    balanceSnapshots = data.balances;
    $('#balanceDialog').close();
    render();
  } catch (error) {
    $('#balanceError').textContent = error.message;
  }
}

function renderChart(items, externalItems) {
  const mode = $('#chartMode').value;
  const context = chartComparisonContext(items, externalItems, mode);
  $('#chartComparisonSummary').hidden = true;
  if (mode === 'deposits') {
    renderDepositsChart(context.items, context);
  } else if (mode === 'net') {
    renderNetIncomeChart(context.externalItems, context);
  } else {
    renderCashflowChart(context.externalItems, context);
  }
}

function accountFilteredTransactions() {
  const account = $('#accountFilter').value;
  return allTransactions.filter(item => account === 'all' || item.account_key === account);
}

function shiftMonthKey(key, offset) {
  const [year, month] = key.split('-').map(Number);
  const shifted = new Date(Date.UTC(year, month - 1 + offset, 1));
  return shifted.toISOString().slice(0, 7);
}

function monthKeyRange(start, end) {
  const keys = [];
  for (let key = start; key <= end; key = shiftMonthKey(key, 1)) keys.push(key);
  return keys;
}

function chartComparisonContext(selectedItems, selectedExternalItems, mode) {
  const comparison = $('#chartComparison')?.value || 'selected';
  const accountItems = accountFilteredTransactions();
  const selectedAccount = $('#accountFilter').value;
  const configuredDepositAccounts = mode === 'deposits'
    ? accounts.filter(account =>
      account.type !== 'credit_card' &&
      (selectedAccount === 'all' || account.key === selectedAccount) &&
      balanceSnapshots[account.key]
    )
    : [];
  const configuredDepositKeys = new Set(configuredDepositAccounts.map(account => account.key));
  const rangeItems = mode === 'deposits'
    ? accountItems.filter(item => configuredDepositKeys.has(item.account_key))
    : accountItems;
  const snapshotDates = mode === 'deposits'
    ? configuredDepositAccounts.map(account => balanceSnapshots[account.key].as_of)
    : [];
  const availableDates = [...rangeItems.map(item => item.date), ...snapshotDates].filter(Boolean).sort();
  const latestDate = availableDates.length ? availableDates[availableDates.length - 1] : '';
  if (comparison === 'selected') {
    return {
      comparison,
      items: selectedItems,
      externalItems: selectedExternalItems,
      monthKeys: null,
      latestDate
    };
  }

  const availableMonths = [...new Set(availableDates.map(date => date.slice(0, 7)))].sort();
  if (!availableMonths.length) {
    return { comparison, items: [], externalItems: [], monthKeys: [], latestDate: '' };
  }
  const latest = availableMonths[availableMonths.length - 1];
  let monthKeys;
  if (comparison === 'previous') {
    monthKeys = [shiftMonthKey(latest, -1), latest];
  } else {
    const latestYear = latest.slice(0, 4);
    const firstAvailable = availableMonths.find(key => key.startsWith(`${latestYear}-`)) || `${latestYear}-01`;
    monthKeys = monthKeyRange(firstAvailable, latest);
  }
  const wanted = new Set(monthKeys);
  const items = rangeItems.filter(item => wanted.has(item.date.slice(0, 7)));
  return {
    comparison,
    items,
    externalItems: items.filter(item => !item.excluded_from_totals),
    monthKeys,
    latestDate
  };
}

function activateChartLegend(activeId) {
  ['cashflowLegend', 'netLegend', 'depositLegend'].forEach(id => {
    $(`#${id}`).hidden = id !== activeId;
  });
}

function monthlyCashflow(items, monthKeys = null) {
  const groups = {};
  items.forEach(item => {
    const key = item.date.slice(0, 7);
    groups[key] ||= { income: 0, spending: 0 };
    groups[key][item.amount > 0 ? 'income' : 'spending'] += Math.abs(item.amount);
  });
  const keys = Array.isArray(monthKeys)
    ? monthKeys
    : Object.keys(groups).sort((left, right) => left.localeCompare(right)).slice(-8);
  return keys.map(key => {
    const values = groups[key] || { income: 0, spending: 0 };
    return { key, ...values, net: values.income - values.spending };
  });
}

function renderChartComparison(context, rows, label, valueForRow) {
  const summary = $('#chartComparisonSummary');
  if (context.comparison === 'selected') {
    summary.hidden = true;
    return;
  }
  summary.hidden = false;
  if (rows.length < 2) {
    summary.innerHTML = '<p>At least two months of data are needed for this comparison.</p>';
    return;
  }
  const baselineRow = rows[0];
  const currentRow = rows[rows.length - 1];
  const baseline = valueForRow(baselineRow);
  const current = valueForRow(currentRow);
  const change = current - baseline;
  const percentage = Math.abs(baseline) > 0.005 ? change / Math.abs(baseline) * 100 : null;
  const changeClass = change > 0 ? 'positive' : change < 0 ? 'negative' : '';
  const arrow = change > 0 ? '↑' : change < 0 ? '↓' : '→';
  const percentageText = percentage === null
    ? (Math.abs(current) > 0.005 ? 'New from a zero baseline' : 'No change')
    : `${Math.abs(percentage).toFixed(1)}% ${change > 0 ? 'higher' : change < 0 ? 'lower' : 'unchanged'}`;
  const rangeLabels = chartMonthLabels([baselineRow, currentRow]);
  const now = new Date();
  const currentMonth = `${now.getFullYear()}-${String(now.getMonth() + 1).padStart(2, '0')}`;
  const isPartialMonth = currentRow.key === currentMonth && context.latestDate?.startsWith(currentMonth);
  const currentPeriodLabel = `${monthLabel(currentRow.key)}${isPartialMonth ? ' to date' : ''}`;
  const currentThrough = isPartialMonth
    ? `<small>Through ${esc(new Date(`${context.latestDate}T12:00:00`).toLocaleDateString('en-GB', { day: 'numeric', month: 'short' }))}</small>`
    : '';
  summary.innerHTML = `<div class="comparison-range"><span>COMPARISON</span><strong>${esc(rangeLabels.join(' – '))}</strong></div>
    <div class="comparison-current"><span>${esc(currentPeriodLabel)} ${esc(label)}</span><strong>${money(current)}</strong>${currentThrough}</div>
    <div class="comparison-change"><span>vs ${esc(monthLabel(baselineRow.key))}</span><strong class="${changeClass}">${arrow} ${money(Math.abs(change))}</strong><small>${esc(percentageText)}</small></div>`;
}

function chartMonthLabels(months) {
  const years = new Set(months.map(item => item.key.slice(0, 4)));
  return months.map(item => new Date(`${item.key}-02T12:00:00`).toLocaleDateString('en-GB',
    years.size > 1 ? { month: 'short', year: '2-digit' } : { month: 'short' }
  ));
}

function svgChartScale(values, signed = false) {
  let minimum = Math.min(0, ...values);
  let maximum = Math.max(0, ...values);
  if (minimum === 0 && maximum === 0) {
    minimum = signed ? -1 : 0;
    maximum = 1;
  }
  const span = maximum - minimum || Math.max(Math.abs(maximum), 1);
  if (signed || minimum < 0) {
    minimum -= span * 0.14;
    maximum += span * 0.14;
  } else {
    maximum += span * 0.18;
  }
  const top = 24;
  const bottom = 188;
  const y = value => top + (maximum - value) / (maximum - minimum) * (bottom - top);
  return { top, bottom, y, zero: y(0) };
}

function svgChartFrame(scale, showZero) {
  const grid = [0, 0.25, 0.5, 0.75, 1].map(step => {
    const y = scale.top + (scale.bottom - scale.top) * step;
    return `<line class="svg-grid-line" x1="58" x2="782" y1="${y}" y2="${y}"></line>`;
  }).join('');
  const zero = showZero
    ? `<line class="svg-zero-line" x1="58" x2="782" y1="${scale.zero}" y2="${scale.zero}"></line>`
    : '';
  return grid + zero;
}

function svgChartX(index, count) {
  if (count === 1) return 420;
  return 70 + index / (count - 1) * 700;
}

function showChartEmpty(message) {
  $('#chart').className = 'chart';
  $('#chart').removeAttribute('aria-label');
  $('#chart').innerHTML = `<p class="chart-empty">${esc(message)}</p>`;
}

function mountSvgChart(svg, ariaLabel) {
  $('#chart').className = 'chart svg-chart';
  $('#chart').setAttribute('aria-label', ariaLabel);
  $('#chart').innerHTML = svg;
}

function renderLineChart(months, series, { title, description, signed = false, fill = false } = {}) {
  if (!months.length) {
    showChartEmpty('No data in this period.');
    return;
  }
  const values = series.flatMap(item => item.values);
  const scale = svgChartScale(values, signed);
  const labels = chartMonthLabels(months);
  const showZero = signed || values.some(value => value < 0);
  const lineShapes = series.map(item => {
    const points = item.values.map((value, index) => `${svgChartX(index, months.length)},${scale.y(value)}`).join(' ');
    const area = fill && item.values.length
      ? `<polygon class="svg-line-area" points="${svgChartX(0, months.length)},${scale.zero} ${points} ${svgChartX(months.length - 1, months.length)},${scale.zero}"></polygon>`
      : '';
    return `${area}<polyline class="svg-series-line" points="${points}" style="stroke:${item.color}"></polyline>`;
  }).join('');
  const points = series.map((item, seriesIndex) => item.values.map((value, index) => {
    const x = svgChartX(index, months.length);
    const y = scale.y(value);
    const color = item.valueColor ? item.valueColor(value) : item.color;
    const horizontalOffset = series.length > 1 ? (seriesIndex === 0 ? -12 : 12) : 0;
    const verticalOffset = signed && value < 0 ? 18 : (series.length > 1 && seriesIndex === 1 ? 17 : -11);
    const valueClass = signed ? (value < 0 ? 'negative-value' : 'positive-value') : '';
    const exact = `${item.label} ${money(value)} in ${monthLabel(months[index].key)}`;
    return `<circle class="svg-series-point" cx="${x}" cy="${y}" r="4.5" style="fill:${color}"><title>${esc(exact)}</title></circle>
      <text class="svg-point-value ${valueClass}" x="${x + horizontalOffset}" y="${y + verticalOffset}" text-anchor="middle">${esc(shortMoney(value))}</text>`;
  }).join('')).join('');
  const monthText = labels.map((label, index) =>
    `<text class="svg-axis-label" x="${svgChartX(index, months.length)}" y="222" text-anchor="middle">${esc(label)}</text>`
  ).join('');
  mountSvgChart(`<svg class="chart-svg" viewBox="0 0 840 230" role="img" aria-labelledby="chartSvgTitle chartSvgDescription">
    <title id="chartSvgTitle">${esc(title)}</title><desc id="chartSvgDescription">${esc(description)}</desc>
    ${svgChartFrame(scale, showZero)}${lineShapes}${points}${monthText}
  </svg>`, title);
}

function renderCashflowChart(items, context) {
  $('#chartEyebrow').textContent = 'CASH FLOW';
  $('#chartTitle').textContent = 'Income & spending';
  activateChartLegend('cashflowLegend');
  const months = monthlyCashflow(items, context.monthKeys);
  renderChartComparison(context, months, 'income', item => item.income);
  if ($('#chartStyle').value === 'line') {
    renderLineChart(months, [
      { label: 'Income', color: '#327658', values: months.map(item => item.income) },
      { label: 'Spending', color: '#e0a17c', values: months.map(item => item.spending) }
    ], {
      title: 'Monthly income and spending',
      description: 'External income and spending for each visible month.'
    });
    return;
  }
  $('#chart').className = `chart${context.comparison === 'selected' ? '' : ' comparison-chart'}`;
  $('#chart').removeAttribute('aria-label');
  const max = Math.max(1, ...months.flatMap(item => [item.income, item.spending]));
  const labels = chartMonthLabels(months);
  $('#chart').innerHTML = months.length ? months.map((item, index) => `
    <div class="month">
      <div class="bar-column">
        <span class="bar-value">${shortMoney(item.income)}</span>
        <div class="bar in" title="External income ${money(item.income)}" style="height:${item.income / max * 100}%"></div>
      </div>
      <div class="bar-column">
        <span class="bar-value">${shortMoney(item.spending)}</span>
        <div class="bar out" title="External spending ${money(item.spending)}" style="height:${item.spending / max * 100}%"></div>
      </div>
      <label>${esc(labels[index])}</label>
    </div>`).join('') : '<p class="chart-empty">No external cash flow in this period.</p>';
}

function renderNetIncomeChart(items, context) {
  $('#chartEyebrow').textContent = 'NET INCOME';
  $('#chartTitle').textContent = 'Monthly net income';
  activateChartLegend('netLegend');
  const months = monthlyCashflow(items, context.monthKeys);
  renderChartComparison(context, months, 'net income', item => item.net);
  const total = months.reduce((sum, item) => sum + item.net, 0);
  $('#netPeriodTotal').textContent = `${total > 0 ? '+' : ''}${money(total)}`;
  $('#netPeriodTotal').className = total < 0 ? 'negative' : 'positive';
  if (!months.length) {
    showChartEmpty('No external income or spending in this period.');
    return;
  }
  if ($('#chartStyle').value === 'line') {
    renderLineChart(months, [{
      label: 'Net income',
      color: '#416b59',
      valueColor: value => value < 0 ? '#c16a58' : '#2f7758',
      values: months.map(item => item.net)
    }], {
      title: 'Monthly net income',
      description: 'Monthly external income minus external spending. Points above zero are a surplus and points below zero are a deficit.',
      signed: true,
      fill: true
    });
    return;
  }
  const values = months.map(item => item.net);
  const scale = svgChartScale(values, true);
  const labels = chartMonthLabels(months);
  const plotWidth = 700;
  const barWidth = Math.min(42, plotWidth / Math.max(months.length, 1) * 0.42);
  const bars = months.map((item, index) => {
    const x = svgChartX(index, months.length);
    const valueY = scale.y(item.net);
    const y = Math.min(valueY, scale.zero);
    const height = Math.max(2, Math.abs(valueY - scale.zero));
    const positive = item.net >= 0;
    const color = positive ? '#327658' : '#cf775f';
    const textY = positive ? y - 9 : y + height + 16;
    return `<rect class="svg-net-bar" x="${x - barWidth / 2}" y="${y}" width="${barWidth}" height="${height}" rx="5" style="fill:${color}"><title>${esc(`Net income ${money(item.net)} in ${monthLabel(item.key)}`)}</title></rect>
      <text class="svg-point-value ${positive ? 'positive-value' : 'negative-value'}" x="${x}" y="${textY}" text-anchor="middle">${esc(`${item.net > 0 ? '+' : ''}${shortMoney(item.net)}`)}</text>
      <text class="svg-axis-label" x="${x}" y="222" text-anchor="middle">${esc(labels[index])}</text>`;
  }).join('');
  mountSvgChart(`<svg class="chart-svg" viewBox="0 0 840 230" role="img" aria-labelledby="chartSvgTitle chartSvgDescription">
    <title id="chartSvgTitle">Monthly net income</title><desc id="chartSvgDescription">Monthly external income minus external spending. Green bars are a surplus and red bars are a deficit.</desc>
    ${svgChartFrame(scale, true)}${bars}
  </svg>`, 'Monthly net income');
}

function renderDepositsChart(items, context) {
  $('#chartEyebrow').textContent = 'BALANCE TREND';
  $('#chartTitle').textContent = 'Monthly total deposits';
  activateChartLegend('depositLegend');
  $('#chart').className = 'chart deposit-chart';

  const selectedAccount = $('#accountFilter').value;
  const bankAccounts = accounts.filter(account =>
    account.type !== 'credit_card' && (selectedAccount === 'all' || account.key === selectedAccount)
  );
  const configuredBankAccounts = bankAccounts.filter(account => balanceSnapshots[account.key]);
  if (!bankAccounts.length) {
    showChartEmpty('Total deposits are only available for bank accounts.');
    return;
  }
  if (!configuredBankAccounts.length) {
    showChartEmpty('Set bank account balances first to show monthly total deposits.');
    return;
  }

  const selectedBankKeys = new Set(bankAccounts.map(account => account.key));
  const monthKeys = Array.isArray(context.monthKeys) ? context.monthKeys : [...new Set(items
    .filter(item => selectedBankKeys.has(item.account_key))
    .map(item => item.date.slice(0, 7)))]
    .sort()
    .slice(-8);
  const snapshotMonths = configuredBankAccounts
    .map(account => balanceSnapshots[account.key].as_of.slice(0, 7));
  const months = monthKeys.length ? monthKeys : [...new Set(snapshotMonths)].sort().slice(-8);
  const latestDate = context.latestDate || allTransactions.reduce((latest, item) => item.date > latest ? item.date : latest, '');
  const series = months.map(key => {
    const asOf = monthEnd(key, latestDate);
    const total = configuredBankAccounts.reduce((sum, account) => {
      const balance = accountBalanceAt(account, asOf);
      return balance === null ? sum : sum + balance;
    }, 0);
    return { key, asOf, total };
  });
  renderChartComparison(context, series, 'deposits', item => item.total);
  if ($('#chartStyle').value === 'line') {
    renderLineChart(series, [{
      label: 'Total deposits',
      color: '#466d9f',
      values: series.map(item => item.total)
    }], {
      title: 'Monthly total deposits',
      description: 'Estimated bank account deposits at the end of each visible month.',
      signed: series.some(item => item.total < 0)
    });
    return;
  }
  $('#chart').className = `chart deposit-chart${context.comparison === 'selected' ? '' : ' comparison-chart'}`;
  const max = Math.max(1, ...series.map(item => Math.abs(item.total)));
  const labels = chartMonthLabels(series);
  $('#chart').innerHTML = series.length ? series.map((item, index) => `
    <div class="month">
      <div class="bar-column wide">
        <span class="bar-value">${shortMoney(item.total)}</span>
        <div class="bar deposit" title="Estimated total deposits ${money(item.total)} as of ${item.asOf}" style="height:${Math.abs(item.total) / max * 100}%"></div>
      </div>
      <label>${esc(labels[index])}</label>
    </div>`).join('') : '<p class="chart-empty">No monthly deposit data in this period.</p>';
}

function categoryScopeItems(items, external, outgoing) {
  const scope = $('#categoryScope')?.value || 'all';
  if (scope === 'spending') {
    $('#categoryEyebrow').textContent = 'SPENDING';
    $('#categoryTitle').textContent = 'Spending by category';
    return { rows: outgoing, empty: 'No external spending in this period.' };
  }
  if (scope === 'external') {
    $('#categoryEyebrow').textContent = 'EXTERNAL CASH FLOW';
    $('#categoryTitle').textContent = 'External by category';
    return { rows: external, empty: 'No external transactions in this period.' };
  }
  $('#categoryEyebrow').textContent = 'CATEGORY MIX';
  $('#categoryTitle').textContent = 'All transactions by category';
  return { rows: items, empty: 'No transactions in this period.' };
}

function renderCategories(items, external, outgoing) {
  const { rows, empty } = categoryScopeItems(items, external, outgoing);
  const groups = {};
  rows.forEach(item => groups[item.category] = (groups[item.category] || 0) + Math.abs(item.amount));
  const sorted = Object.entries(groups).sort((a, b) => b[1] - a[1]);
  const total = sorted.reduce((sum, item) => sum + item[1], 0) || 1;
  const visibleLimit = 4;
  const visible = categoryExpanded ? sorted : sorted.slice(0, visibleLimit);
  const hiddenCount = Math.max(0, sorted.length - visible.length);
  $('#categories').classList.toggle('expanded', categoryExpanded);
  $('#categories').innerHTML = visible.map(([name, value]) => `
    <div class="cat-row"><strong>${esc(name)}</strong><span>${money(value)}</span>
      <div class="track"><i style="width:${value / total * 100}%"></i></div>
    </div>`).join('') || `<p>${empty}</p>`;
  if (sorted.length > visibleLimit) {
    $('#categories').insertAdjacentHTML('beforeend', `<button id="categoryToggle" class="category-toggle">${categoryExpanded ? 'Show less' : `Show all ${sorted.length} categories`}</button>`);
    $('#categoryToggle').onclick = () => {
      categoryExpanded = !categoryExpanded;
      render();
    };
  }
}

function categorySourceLabel(source) {
  return {
    manual: 'manual',
    learned_amount: 'learned',
    auto: 'auto'
  }[source] || 'auto';
}

async function saveTransactionCategory(event) {
  const select = event.target;
  const fingerprint = select.dataset.fingerprint;
  const previous = select.dataset.current;
  let category = select.value;
  if (category === '__custom__') {
    category = prompt('New category name', previous || 'Other');
    if (!category || !category.trim()) {
      select.value = previous;
      return;
    }
    category = category.trim();
  }
  const transaction = allTransactions.find(item => item.transaction_fingerprint === fingerprint);
  if (!transaction) return;
  select.disabled = true;
  try {
    const data = await request('/api/category-rule', {
      method: 'POST',
      body: JSON.stringify({
        dataset: $('#datasetFilter').value,
        transaction_fingerprint: fingerprint,
        category,
        learn_amount_rule: true
      })
    });
    categoryOptions = data.category_options || categoryOptions;
    const learnedKey = transaction.amount_rule_key;
    allTransactions = allTransactions.map(item => {
      if (item.transaction_fingerprint === fingerprint) {
        return { ...item, category, category_source: 'manual' };
      }
      if (item.amount_rule_key === learnedKey && item.category_source !== 'manual') {
        return { ...item, category, category_source: 'learned_amount' };
      }
      return item;
    });
    render();
  } catch (error) {
    alert(error.message);
    select.disabled = false;
    select.value = previous;
  }
}

function typeLabel(item) {
  if (item.internal_reason === 'credit_card_repayment' || item.internal_reason === 'credit_card_payment_received') return ['Card repayment', 'card-badge'];
  if (item.transfer_type === 'internal') return ['Internal transfer', 'internal-badge'];
  if (item.transfer_type === 'external') return ['External transfer', 'external-badge'];
  return item.amount > 0 ? ['Income', 'income-badge'] : ['Expense', 'expense-badge'];
}

function transactionTypeKey(item) {
  if (item.internal_reason === 'credit_card_repayment' || item.internal_reason === 'credit_card_payment_received') return 'card_repayment';
  if (item.transfer_type === 'internal') return 'internal';
  if (item.transfer_type === 'external') return 'external';
  return item.amount > 0 ? 'income' : 'expense';
}

function monthLabel(key) {
  return new Date(`${key}-02T12:00:00`).toLocaleDateString('en-GB', { month: 'long', year: 'numeric' });
}

function monthSummary(items) {
  const income = items.filter(item => item.amount > 0).reduce((sum, item) => sum + item.amount, 0);
  const spending = -items.filter(item => item.amount < 0).reduce((sum, item) => sum + item.amount, 0);
  return { income, spending, net: income - spending };
}

function renderTransactionRow(item) {
  const [label, badgeClass] = typeLabel(item);
  const description = item.transfer_type === 'internal' && item.counterparty_account
    ? `${item.memo} · ${item.counterparty_account}` : item.memo;
  const options = [...new Set([...categoryOptions, item.category])].sort((a, b) => a.localeCompare(b)).map(category =>
    `<option value="${esc(category)}" ${category === item.category ? 'selected' : ''}>${esc(category)}</option>`
  ).join('');
  return `<tr class="${item.transfer_type === 'internal' ? 'internal-row' : ''}">
    <td>${new Date(`${item.date}T12:00:00`).toLocaleDateString('en-GB', { day:'2-digit', month:'short', year:'numeric' })}</td>
    <td class="memo" title="${esc(description)}">${esc(description || 'Bank transaction')}</td>
    <td><span class="type-badge ${badgeClass}">${label}</span></td>
    <td><select class="category-select" data-fingerprint="${esc(item.transaction_fingerprint)}" data-current="${esc(item.category)}">${options}<option value="__custom__">Custom…</option></select><small class="category-source">${categorySourceLabel(item.category_source)}</small></td>
    <td><strong>${esc(item.account_label)}</strong><small class="source-name">${esc(item.source)}</small></td>
    <td class="${item.transfer_type === 'internal' ? 'internal-amount' : item.amount >= 0 ? 'positive' : 'negative'}">${item.amount >= 0 ? '+' : ''}${money(item.amount)}</td>
  </tr>`;
}

function setupTransactionCategoryFilter(items) {
  const select = $('#transactionCategoryFilter');
  if (!select) return;
  const selected = select.value || 'all';
  const categories = [...new Set(items.map(item => item.category).filter(Boolean))]
    .sort((a, b) => a.localeCompare(b));
  select.innerHTML = '<option value="all">All categories</option>' + categories.map(category =>
    `<option value="${esc(category)}">${esc(category)}</option>`
  ).join('');
  select.value = categories.includes(selected) ? selected : 'all';
}

function transactionSummaryText(items) {
  const income = items.filter(item => item.amount > 0).reduce((sum, item) => sum + item.amount, 0);
  const spending = -items.filter(item => item.amount < 0).reduce((sum, item) => sum + item.amount, 0);
  const net = income - spending;
  const absolute = items.reduce((sum, item) => sum + Math.abs(item.amount), 0);
  const count = `${items.length} transaction${items.length === 1 ? '' : 's'}`;
  return `${count} · Net ${money(net)} · Income ${money(income)} · Spending ${money(spending)} · Total ${money(absolute)}`;
}

function renderRows(items) {
  setupTransactionCategoryFilter(items);
  const query = $('#search').value.toLowerCase();
  const type = $('#transactionTypeFilter')?.value || 'all';
  const category = $('#transactionCategoryFilter')?.value || 'all';
  const shown = items.filter(item =>
    (type === 'all' || transactionTypeKey(item) === type) &&
    (category === 'all' || item.category === category) &&
    `${item.memo} ${item.category} ${item.account_label} ${item.source} ${item.transfer_type}`.toLowerCase().includes(query)
  );
  $('#transactionSummary').textContent = transactionSummaryText(shown);
  if (!shown.length) {
    $('#transactionRows').innerHTML = '<tr><td colspan="6">No matching transactions.</td></tr>';
    return;
  }

  const groups = new Map();
  shown.forEach(item => {
    const key = item.date.slice(0, 7);
    if (!groups.has(key)) groups.set(key, []);
    groups.get(key).push(item);
  });
  const monthKeys = [...groups.keys()];
  if (query) {
    collapsedMonths = new Set();
  } else if (!monthCollapseInitialised) {
    collapsedMonths = new Set(monthKeys.slice(1));
    monthCollapseInitialised = true;
  }

  $('#transactionRows').innerHTML = monthKeys.map(key => {
    const rows = groups.get(key);
    const summary = monthSummary(rows);
    const collapsed = collapsedMonths.has(key);
    const header = `<tr class="month-group-row">
      <td colspan="6"><button class="month-toggle" data-month="${esc(key)}">
        <span>${collapsed ? '▸' : '▾'} ${esc(monthLabel(key))}</span>
        <small>${rows.length} transaction${rows.length === 1 ? '' : 's'} · Income ${money(summary.income)} · Spending ${money(summary.spending)} · Net ${money(summary.net)}</small>
      </button></td>
    </tr>`;
    return header + (collapsed ? '' : rows.map(renderTransactionRow).join(''));
  }).join('');
  document.querySelectorAll('.month-toggle').forEach(button => button.onclick = () => {
    const month = button.dataset.month;
    if (collapsedMonths.has(month)) collapsedMonths.delete(month);
    else collapsedMonths.add(month);
    renderRows(items);
  });
  document.querySelectorAll('.category-select').forEach(select => select.onchange = saveTransactionCategory);
}

$('#datasetFilter').onchange = loadDashboard;
$('#accountFilter').onchange = render;
$('#periodFilter').onchange = render;
$('#chartMode').onchange = render;
$('#chartComparison').onchange = render;
$('#chartStyle').onchange = render;
$('#categoryScope').onchange = () => {
  categoryExpanded = false;
  render();
};
$('#search').oninput = render;
$('#transactionTypeFilter').onchange = () => {
  monthCollapseInitialised = false;
  render();
};
$('#transactionCategoryFilter').onchange = () => {
  monthCollapseInitialised = false;
  render();
};
$('#balanceButton').onclick = openBalanceDialog;
$('#closeBalanceDialog').onclick = () => $('#balanceDialog').close();
$('#cancelBalances').onclick = () => $('#balanceDialog').close();
$('#balanceForm').onsubmit = saveBalances;
$('#closeMetricDialog').onclick = () => $('#metricDialog').close();
document.querySelectorAll('[data-detail]').forEach(card => card.onclick = () => openMetricDialog(card.dataset.detail));
document.querySelectorAll('[data-view-target]').forEach(button => button.onclick = () => {
  const view = button.dataset.viewTarget;
  if (window.location.hash === `#${view}`) setView(view);
  else window.location.hash = view;
});
window.addEventListener('hashchange', () => setView(viewFromHash()));
setView(viewFromHash());
loadDashboard();
