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
    $('#fileCount').textContent = `${data.file_count} CSV file${data.file_count === 1 ? '' : 's'} · ${accounts.length} account${accounts.length === 1 ? '' : 's'}`;
    render();
  } catch (error) {
    allTransactions = [];
    $('#transactionRows').innerHTML = `<tr><td colspan="6">${esc(error.message)}</td></tr>`;
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
  const newest = allTransactions.length ? Math.max(...allTransactions.map(item => new Date(item.date))) : 0;
  return allTransactions.filter(item =>
    (account === 'all' || item.account_key === account) &&
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
    renderPortfolio();
  } catch (error) {
    $('#balanceError').textContent = error.message;
  }
}

function renderChart(items, externalItems) {
  if ($('#chartMode').value === 'deposits') {
    renderDepositsChart(items);
  } else {
    renderCashflowChart(externalItems);
  }
}

function renderCashflowChart(items) {
  $('#chartEyebrow').textContent = 'CASH FLOW';
  $('#chartTitle').textContent = 'Income & spending';
  $('#cashflowLegend').hidden = false;
  $('#depositLegend').hidden = true;
  $('#chart').className = 'chart';
  const groups = {};
  items.forEach(item => {
    const key = item.date.slice(0, 7);
    groups[key] ||= { in: 0, out: 0 };
    groups[key][item.amount > 0 ? 'in' : 'out'] += Math.abs(item.amount);
  });
  const months = Object.entries(groups).sort().slice(-8);
  const max = Math.max(1, ...months.flatMap(([, values]) => [values.in, values.out]));
  $('#chart').innerHTML = months.length ? months.map(([key, values]) => `
    <div class="month">
      <div class="bar-column">
        <span class="bar-value">${shortMoney(values.in)}</span>
        <div class="bar in" title="External income ${money(values.in)}" style="height:${values.in / max * 100}%"></div>
      </div>
      <div class="bar-column">
        <span class="bar-value">${shortMoney(values.out)}</span>
        <div class="bar out" title="External spending ${money(values.out)}" style="height:${values.out / max * 100}%"></div>
      </div>
      <label>${new Date(`${key}-02`).toLocaleDateString('en-GB', { month: 'short' })}</label>
    </div>`).join('') : '<p>No external cash flow.</p>';
}

function renderDepositsChart(items) {
  $('#chartEyebrow').textContent = 'BALANCE TREND';
  $('#chartTitle').textContent = 'Monthly total deposits';
  $('#cashflowLegend').hidden = true;
  $('#depositLegend').hidden = false;
  $('#chart').className = 'chart deposit-chart';

  const selectedAccount = $('#accountFilter').value;
  const bankAccounts = accounts.filter(account =>
    account.type !== 'credit_card' && (selectedAccount === 'all' || account.key === selectedAccount)
  );
  const configuredBankAccounts = bankAccounts.filter(account => balanceSnapshots[account.key]);
  if (!bankAccounts.length) {
    $('#chart').innerHTML = '<p class="chart-empty">Total deposits are only available for bank accounts.</p>';
    return;
  }
  if (!configuredBankAccounts.length) {
    $('#chart').innerHTML = '<p class="chart-empty">Set bank account balances first to show monthly total deposits.</p>';
    return;
  }

  const selectedBankKeys = new Set(bankAccounts.map(account => account.key));
  const monthKeys = [...new Set(items
    .filter(item => selectedBankKeys.has(item.account_key))
    .map(item => item.date.slice(0, 7)))]
    .sort()
    .slice(-8);
  const snapshotMonths = configuredBankAccounts
    .map(account => balanceSnapshots[account.key].as_of.slice(0, 7));
  const months = monthKeys.length ? monthKeys : [...new Set(snapshotMonths)].sort().slice(-8);
  const latestDate = allTransactions.reduce((latest, item) => item.date > latest ? item.date : latest, '');
  const series = months.map(key => {
    const asOf = monthEnd(key, latestDate);
    const total = configuredBankAccounts.reduce((sum, account) => {
      const balance = accountBalanceAt(account, asOf);
      return balance === null ? sum : sum + balance;
    }, 0);
    return { key, asOf, total };
  });
  const max = Math.max(1, ...series.map(item => Math.abs(item.total)));
  $('#chart').innerHTML = series.length ? series.map(item => `
    <div class="month">
      <div class="bar-column wide">
        <span class="bar-value">${shortMoney(item.total)}</span>
        <div class="bar deposit" title="Estimated total deposits ${money(item.total)} as of ${item.asOf}" style="height:${Math.abs(item.total) / max * 100}%"></div>
      </div>
      <label>${new Date(`${item.key}-02`).toLocaleDateString('en-GB', { month: 'short' })}</label>
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
loadDashboard();
