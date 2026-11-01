(function initialiseAnalyticsWidget() {
  const widget = document.querySelector('#analyticsWidget');
  const panel = document.querySelector('#analyticsWidgetPanel');
  const launcher = document.querySelector('#analyticsPetLauncher');
  const sprite = document.querySelector('#analyticsPetSprite');
  const unread = document.querySelector('#analyticsUnread');
  const closeButton = document.querySelector('#analyticsClose');
  const settingsToggle = document.querySelector('#analyticsSettingsToggle');
  const settingsPanel = document.querySelector('#analyticsSettings');
  const form = document.querySelector('#analyticsForm');
  if (!widget || !panel || !launcher || !sprite || !form) return;

  const status = document.querySelector('#analyticsStatus');
  const messages = document.querySelector('#analyticsMessages');
  const emptyState = document.querySelector('#analyticsEmpty');
  const questionInput = document.querySelector('#analyticsQuestion');
  const submitButton = document.querySelector('#analyticsSubmit');
  const errorMessage = document.querySelector('#analyticsError');
  const contextLabel = document.querySelector('#analyticsContext');
  const settingsForm = document.querySelector('#analyticsSettingsForm');
  const apiKeyInput = document.querySelector('#analyticsApiKey');
  const saveKeyButton = document.querySelector('#analyticsSaveKey');
  const removeKeyButton = document.querySelector('#analyticsRemoveKey');
  const settingsMessage = document.querySelector('#analyticsSettingsMessage');
  const suggestionButtons = [...document.querySelectorAll('[data-analytics-question]')];
  const datasetFilter = document.querySelector('#datasetFilter');
  const accountFilter = document.querySelector('#accountFilter');
  const currencyFormatters = new Map();
  const reducedMotion = window.matchMedia('(prefers-reduced-motion: reduce)');
  const petStates = {
    idle: { row: 0, frames: 6, speed: 220 },
    'running-right': { row: 1, frames: 8, speed: 120 },
    'running-left': { row: 2, frames: 8, speed: 120 },
    waving: { row: 3, frames: 4, speed: 180 },
    jumping: { row: 4, frames: 5, speed: 130 },
    failed: { row: 5, frames: 8, speed: 190 },
    waiting: { row: 6, frames: 6, speed: 230 },
    running: { row: 7, frames: 6, speed: 135 },
    review: { row: 8, frames: 6, speed: 185 }
  };

  let endpointAvailable = false;
  let apiConfigured = false;
  let dashboardReady = window.__dashboardDataReady === true;
  let pending = false;
  let widgetOpen = false;
  let spriteReady = false;
  let petState = 'idle';
  let petTimer = null;
  let petTransitionTimer = null;
  let pendingController = null;
  let requestEpoch = 0;
  let conversationId = null;
  let previousFocus = null;

  function formatter(currency) {
    const safeCurrency = /^[A-Z]{3}$/.test(currency || '') ? currency : 'GBP';
    if (!currencyFormatters.has(safeCurrency)) {
      currencyFormatters.set(safeCurrency, new Intl.NumberFormat('en-GB', {
        style: 'currency', currency: safeCurrency, maximumFractionDigits: 2
      }));
    }
    return currencyFormatters.get(safeCurrency);
  }

  function setPetFrame(row, frame) {
    sprite.style.setProperty('--pet-row', String(row));
    sprite.style.setProperty('--pet-frame', String(frame));
  }

  function setPetState(name) {
    const config = petStates[name] || petStates.idle;
    window.clearInterval(petTimer);
    window.clearTimeout(petTransitionTimer);
    petTimer = null;
    petTransitionTimer = null;
    petState = name in petStates ? name : 'idle';
    widget.dataset.petState = petState;
    setPetFrame(config.row, 0);
    if (!spriteReady || reducedMotion.matches || config.frames < 2) return;
    let frame = 0;
    petTimer = window.setInterval(() => {
      frame = (frame + 1) % config.frames;
      setPetFrame(config.row, frame);
    }, config.speed);
  }

  function restingPetState() {
    if (pending) return 'running';
    if (!endpointAvailable) return 'failed';
    if (!apiConfigured || widgetOpen) return 'waiting';
    if (!unread.hidden) return 'waving';
    return 'idle';
  }

  function showPetStateBriefly(name, duration) {
    setPetState(name);
    petTransitionTimer = window.setTimeout(() => setPetState(restingPetState()), duration);
  }

  function loadPetSprite() {
    const image = new Image();
    image.onload = () => {
      if (image.naturalWidth !== 1536 || image.naturalHeight !== 1872) {
        sprite.classList.add('is-fallback');
        return;
      }
      spriteReady = true;
      sprite.style.backgroundImage = 'url("/assets/pet/spritesheet")';
      sprite.classList.add('is-sprite');
      setPetState(petState);
    };
    image.onerror = () => sprite.classList.add('is-fallback');
    image.src = '/assets/pet/spritesheet';
  }

  function setStatus(kind, message) {
    status.className = `analytics-status ${kind}`;
    const dot = document.createElement('i');
    dot.setAttribute('aria-hidden', 'true');
    const copy = document.createElement('span');
    copy.textContent = message;
    status.replaceChildren(dot, copy);
  }

  function setControlsEnabled(enabled) {
    questionInput.disabled = !enabled || pending;
    submitButton.disabled = !enabled || pending;
    suggestionButtons.forEach(button => { button.disabled = !enabled || pending; });
  }

  function setSettingsBusy(busy) {
    apiKeyInput.disabled = busy;
    saveKeyButton.disabled = busy;
    removeKeyButton.disabled = busy || !apiConfigured;
  }

  function applyProviderStatus(data) {
    apiConfigured = Boolean(data.configured);
    removeKeyButton.disabled = !apiConfigured;
    if (apiConfigured) {
      const model = typeof data.model === 'string' && data.model.trim() ? ` · ${data.model.trim()}` : '';
      setStatus('is-ready', `DeepSeek ready${model}`);
    } else {
      setStatus('is-local', 'API not configured · local fallback remains available');
    }
    if (!pending) setPetState(restingPetState());
  }

  async function fetchJson(url, options = {}, timeout = 45000, suppliedController = null) {
    const controller = suppliedController || new AbortController();
    const timeoutId = window.setTimeout(() => controller.abort(), timeout);
    try {
      const response = await fetch(url, {
        ...options,
        signal: controller.signal,
        headers: options.body
          ? { 'content-type': 'application/json', ...(options.headers || {}) }
          : (options.headers || {})
      });
      const contentType = response.headers.get('content-type') || '';
      const data = contentType.includes('application/json') ? await response.json() : {};
      if (!response.ok) throw new Error(data.error || 'Analytics request failed.');
      return data;
    } catch (error) {
      if (error.name === 'AbortError') throw new Error('Analytics took too long to respond. Please try again.');
      throw error;
    } finally {
      window.clearTimeout(timeoutId);
    }
  }

  async function loadStatus() {
    setControlsEnabled(false);
    setStatus('is-loading', 'Checking Analytics availability…');
    try {
      const data = await fetchJson('/api/analytics/status', {}, 10000);
      endpointAvailable = true;
      applyProviderStatus(data);
    } catch (_error) {
      endpointAvailable = false;
      setStatus('is-error', 'Analytics is unavailable while the local service starts');
      setPetState('failed');
    } finally {
      setControlsEnabled(endpointAvailable && dashboardReady);
    }
  }

  function selectedScope() {
    const selectedAccount = accountFilter.value === 'all' ? null : accountFilter.value;
    return {
      dataset: datasetFilter.value,
      accountScope: selectedAccount === null ? 'all' : 'selected',
      selectedAccount
    };
  }

  function scopeKey() {
    const scope = selectedScope();
    return `${scope.dataset}\u0000${scope.selectedAccount || 'all'}`;
  }

  function updateContextLabel() {
    const selected = accountFilter.value;
    const accountName = accountFilter.selectedOptions[0]?.textContent || 'Selected account';
    contextLabel.textContent = selected === 'all' ? 'All accounts' : accountName;
    contextLabel.title = contextLabel.textContent;
  }

  function clearTurn() {
    messages.replaceChildren(emptyState);
    emptyState.hidden = false;
    errorMessage.textContent = '';
  }

  function setUnread(available) {
    unread.hidden = !available;
    launcher.setAttribute('aria-label', available
      ? 'Open deepseek酱 financial assistant. New answer available.'
      : 'Open deepseek酱 financial assistant');
  }

  function resetConversationForScope() {
    requestEpoch += 1;
    if (pendingController) pendingController.abort();
    pendingController = null;
    pending = false;
    conversationId = null;
    setUnread(false);
    clearTurn();
    setControlsEnabled(endpointAvailable && dashboardReady);
    updateContextLabel();
    setPetState(restingPetState());
  }

  function openWidget() {
    if (widgetOpen) {
      (questionInput.disabled ? closeButton : questionInput).focus();
      return;
    }
    const activeElement = document.activeElement;
    previousFocus = activeElement instanceof HTMLElement && activeElement !== document.body
      ? activeElement
      : launcher;
    widgetOpen = true;
    panel.hidden = false;
    widget.classList.add('is-open');
    launcher.setAttribute('aria-expanded', 'true');
    setUnread(false);
    showPetStateBriefly('jumping', 560);
    window.requestAnimationFrame(() => {
      (questionInput.disabled ? closeButton : questionInput).focus();
    });
  }

  function closeWidget() {
    if (!widgetOpen) return;
    widgetOpen = false;
    panel.hidden = true;
    widget.classList.remove('is-open');
    launcher.setAttribute('aria-expanded', 'false');
    settingsPanel.hidden = true;
    settingsToggle.setAttribute('aria-expanded', 'false');
    setPetState(restingPetState());
    const focusTarget = previousFocus instanceof HTMLElement && document.contains(previousFocus)
      ? previousFocus
      : launcher;
    focusTarget.focus();
    previousFocus = null;
  }

  function toggleSettings(force) {
    const open = typeof force === 'boolean' ? force : settingsPanel.hidden;
    settingsPanel.hidden = !open;
    settingsToggle.setAttribute('aria-expanded', String(open));
    if (open) window.requestAnimationFrame(() => apiKeyInput.focus());
  }

  function appendUserMessage(text) {
    emptyState.hidden = true;
    const message = document.createElement('article');
    message.className = 'analytics-message user';
    const label = document.createElement('div');
    label.className = 'analytics-message-label';
    label.textContent = 'You';
    const body = document.createElement('div');
    body.className = 'analytics-message-body';
    body.textContent = text;
    message.append(label, body);
    messages.append(message);
  }

  function humanLabel(value) {
    return String(value || 'Result')
      .replace(/_minor$/i, '')
      .replace(/[_-]+/g, ' ')
      .replace(/\b\w/g, letter => letter.toUpperCase());
  }

  function primitiveDisplay(key, value, currency) {
    if (value === null || value === undefined) return 'Not available';
    if (typeof value === 'boolean') return value ? 'Yes' : 'No';
    if (typeof value === 'number') {
      if (/_minor$/i.test(key)) return formatter(currency).format(value / 100);
      if (/percent|percentage|_pct$/i.test(key)) {
        return `${value.toLocaleString('en-GB', { maximumFractionDigits: 1 })}%`;
      }
      return value.toLocaleString('en-GB', { maximumFractionDigits: 2 });
    }
    return String(value);
  }

  function collectFactValues(value, path, rows, currency) {
    if (rows.length >= 18) return;
    if (value === null || value === undefined || ['string', 'number', 'boolean'].includes(typeof value)) {
      const key = path[path.length - 1] || 'result';
      rows.push({ label: path.map(humanLabel).join(' · '), value: primitiveDisplay(key, value, currency) });
      return;
    }
    if (Array.isArray(value)) {
      value.forEach((item, index) => {
        const identity = item && typeof item === 'object'
          ? item.period || item.month || item.category || item.label || item.name
          : null;
        collectFactValues(item, [...path, identity || `Item ${index + 1}`], rows, currency);
      });
      return;
    }
    if (typeof value === 'object') {
      Object.entries(value).forEach(([key, child]) => {
        if (key !== 'currency' && key !== 'definition') collectFactValues(child, [...path, key], rows, currency);
      });
    }
  }

  function factTitle(name) {
    return ({
      get_period_summary: 'Period summary',
      compare_financial_metric: 'Period comparison',
      get_monthly_metric_series: 'Monthly trend',
      get_category_breakdown: 'Category breakdown'
    })[name] || humanLabel(name || 'Local calculation');
  }

  function renderFacts(facts) {
    if (!Array.isArray(facts) || !facts.length) return null;
    const wrapper = document.createElement('details');
    wrapper.className = 'analytics-facts';
    const heading = document.createElement('summary');
    heading.className = 'analytics-facts-heading';
    heading.textContent = `Locally verified facts (${facts.length})`;
    wrapper.append(heading);
    const cards = document.createElement('div');
    cards.className = 'analytics-fact-cards';

    facts.forEach(fact => {
      const card = document.createElement('article');
      card.className = 'analytics-fact-card';
      const title = document.createElement('h4');
      title.textContent = factTitle(fact?.name);
      card.append(title);
      const result = fact && Object.prototype.hasOwnProperty.call(fact, 'result') ? fact.result : fact;
      const currency = result && typeof result === 'object' && typeof result.currency === 'string'
        ? result.currency.toUpperCase()
        : 'GBP';
      const values = [];
      collectFactValues(result, [], values, currency);
      const grid = document.createElement('div');
      grid.className = 'analytics-fact-grid';
      (values.length ? values : [{ label: 'Result', value: 'Calculated locally' }]).forEach(item => {
        const cell = document.createElement('div');
        cell.className = 'analytics-fact-value';
        const label = document.createElement('span');
        label.textContent = item.label || 'Result';
        const displayedValue = document.createElement('strong');
        displayedValue.textContent = item.value;
        cell.append(label, displayedValue);
        grid.append(cell);
      });
      card.append(grid);
      cards.append(card);
    });
    wrapper.append(cards);
    return wrapper;
  }

  function modeLabel(mode) {
    if (mode === 'deepseek') return 'DeepSeek explanation';
    if (mode === 'local' || mode === 'fallback') return 'Local fallback';
    return typeof mode === 'string' && mode.trim() ? humanLabel(mode) : 'Local analytics';
  }

  function usageLabel(usage) {
    if (!usage || typeof usage !== 'object') return '';
    const total = usage.total_tokens ?? usage.totalTokens
      ?? (Number(usage.input_tokens || 0) + Number(usage.output_tokens || 0));
    return Number.isFinite(Number(total)) ? `${Number(total).toLocaleString('en-GB')} tokens` : '';
  }

  function appendAssistantMessage(data) {
    const message = document.createElement('article');
    message.className = 'analytics-message assistant';
    const label = document.createElement('div');
    label.className = 'analytics-message-label';
    label.textContent = 'deepseek酱';
    const body = document.createElement('div');
    body.className = 'analytics-message-body';
    body.textContent = typeof data.answer === 'string' && data.answer.trim()
      ? data.answer.trim()
      : 'The local calculations completed, but no explanation was returned.';
    message.append(label, body);

    const meta = document.createElement('div');
    meta.className = 'analytics-answer-meta';
    const mode = document.createElement('span');
    mode.textContent = modeLabel(data.mode);
    meta.append(mode);
    if (data.context_used === true) {
      const contextUsed = document.createElement('span');
      contextUsed.textContent = 'Previous question understood';
      meta.append(contextUsed);
    }
    const usage = usageLabel(data.usage);
    if (usage) {
      const usageItem = document.createElement('span');
      usageItem.textContent = usage;
      meta.append(usageItem);
    }
    message.append(meta);

    if (typeof data.warning === 'string' && data.warning.trim()) {
      const warning = document.createElement('div');
      warning.className = 'analytics-warning';
      warning.textContent = data.warning.trim();
      message.append(warning);
    }
    const facts = renderFacts(data.facts);
    if (facts) message.append(facts);
    messages.append(message);
    scrollToLatest();
  }

  function addLoadingMessage() {
    const row = document.createElement('div');
    row.className = 'analytics-loading-row';
    row.setAttribute('aria-label', 'Calculating locally');
    const loading = document.createElement('div');
    loading.className = 'analytics-loading';
    for (let index = 0; index < 3; index += 1) loading.append(document.createElement('i'));
    row.append(loading);
    messages.append(row);
    scrollToLatest();
    return row;
  }

  function scrollToLatest() {
    window.requestAnimationFrame(() => { messages.scrollTop = messages.scrollHeight; });
  }

  async function submitQuestion(question) {
    const cleanedQuestion = String(question || '').trim();
    if (!cleanedQuestion || pending || !endpointAvailable || !dashboardReady) return;
    const epoch = ++requestEpoch;
    const startingScopeKey = scopeKey();
    const scope = selectedScope();
    const controller = new AbortController();
    pendingController = controller;
    pending = true;
    setControlsEnabled(true);
    errorMessage.textContent = '';
    messages.replaceChildren();
    appendUserMessage(cleanedQuestion);
    questionInput.value = '';
    const loading = addLoadingMessage();
    setPetState('running');

    try {
      const data = await fetchJson('/api/analytics/chat', {
        method: 'POST',
        body: JSON.stringify({
          dataset: scope.dataset,
          question: cleanedQuestion,
          account_scope: scope.accountScope,
          selected_account: scope.selectedAccount,
          conversation_id: conversationId
        })
      }, 45000, controller);
      if (epoch !== requestEpoch || scopeKey() !== startingScopeKey) return;
      loading.remove();
      if (typeof data.conversation_id === 'string' && data.conversation_id.trim()) {
        conversationId = data.conversation_id.trim();
      }
      setPetState('review');
      appendAssistantMessage(data);
      if (widgetOpen) {
        petTransitionTimer = window.setTimeout(() => showPetStateBriefly('waving', 900), 450);
      } else {
        setUnread(true);
        setPetState('waving');
      }
    } catch (error) {
      if (epoch !== requestEpoch || scopeKey() !== startingScopeKey) return;
      loading.remove();
      errorMessage.textContent = error.message || 'Analytics could not answer that question.';
      showPetStateBriefly('failed', 1800);
    } finally {
      if (epoch === requestEpoch) {
        pending = false;
        pendingController = null;
        setControlsEnabled(endpointAvailable && dashboardReady);
        if (widgetOpen) questionInput.focus();
      }
    }
  }

  launcher.addEventListener('click', openWidget);
  launcher.addEventListener('pointerenter', () => { if (!pending) setPetState('waving'); });
  launcher.addEventListener('pointerleave', () => { if (!pending) setPetState(restingPetState()); });
  launcher.addEventListener('focus', () => { if (!pending) setPetState('waving'); });
  launcher.addEventListener('blur', () => { if (!pending) setPetState(restingPetState()); });
  closeButton.addEventListener('click', closeWidget);
  settingsToggle.addEventListener('click', () => toggleSettings());

  document.addEventListener('keydown', event => {
    if (event.key === 'Escape' && widgetOpen && !document.querySelector('dialog[open]')) {
      event.preventDefault();
      closeWidget();
    }
  });

  form.addEventListener('submit', event => {
    event.preventDefault();
    submitQuestion(questionInput.value);
  });

  questionInput.addEventListener('keydown', event => {
    if (event.key === 'Enter' && !event.shiftKey && !event.isComposing) {
      event.preventDefault();
      form.requestSubmit();
    }
  });

  suggestionButtons.forEach(button => button.addEventListener('click', () => {
    submitQuestion(button.dataset.analyticsQuestion);
  }));

  settingsForm.addEventListener('submit', async event => {
    event.preventDefault();
    const apiKey = apiKeyInput.value.trim();
    if (!apiKey) {
      settingsMessage.className = 'is-error';
      settingsMessage.textContent = 'Paste a DeepSeek API key first.';
      return;
    }
    setSettingsBusy(true);
    settingsMessage.className = '';
    settingsMessage.textContent = 'Saving locally…';
    try {
      const data = await fetchJson('/api/analytics/settings', {
        method: 'POST', body: JSON.stringify({ api_key: apiKey })
      }, 10000);
      apiKeyInput.value = '';
      applyProviderStatus(data);
      settingsMessage.className = 'is-success';
      settingsMessage.textContent = 'Saved on this Mac.';
    } catch (error) {
      settingsMessage.className = 'is-error';
      settingsMessage.textContent = error.message || 'The API key could not be saved.';
    } finally {
      setSettingsBusy(false);
    }
  });

  removeKeyButton.addEventListener('click', async () => {
    if (!apiConfigured || !window.confirm('Remove the locally stored DeepSeek API key?')) return;
    setSettingsBusy(true);
    settingsMessage.className = '';
    settingsMessage.textContent = 'Removing local key…';
    try {
      const data = await fetchJson('/api/analytics/settings', { method: 'DELETE' }, 10000);
      apiKeyInput.value = '';
      applyProviderStatus(data);
      settingsMessage.className = 'is-success';
      settingsMessage.textContent = 'The local API key was removed.';
    } catch (error) {
      settingsMessage.className = 'is-error';
      settingsMessage.textContent = error.message || 'The local API key could not be removed.';
    } finally {
      setSettingsBusy(false);
    }
  });

  accountFilter.addEventListener('change', resetConversationForScope);
  datasetFilter.addEventListener('change', resetConversationForScope);
  window.addEventListener('dashboard:loading', () => {
    dashboardReady = false;
    resetConversationForScope();
  });
  window.addEventListener('dashboard:data-loaded', () => {
    dashboardReady = true;
    resetConversationForScope();
  });
  window.addEventListener('dashboard:data-failed', () => {
    dashboardReady = false;
    resetConversationForScope();
  });
  window.addEventListener('analytics:open-request', openWidget);

  const dialogs = [...document.querySelectorAll('dialog')];
  function syncDialogVisibility() {
    widget.classList.toggle('is-dialog-obscured', dialogs.some(dialog => dialog.open));
  }
  const dialogObserver = new MutationObserver(syncDialogVisibility);
  dialogs.forEach(dialog => dialogObserver.observe(dialog, { attributes: true, attributeFilter: ['open'] }));

  if (typeof reducedMotion.addEventListener === 'function') {
    reducedMotion.addEventListener('change', () => setPetState(petState));
  }

  unread.setAttribute('aria-hidden', 'true');
  updateContextLabel();
  setSettingsBusy(false);
  setPetState('idle');
  loadPetSprite();
  loadStatus();
  if (window.__openAnalyticsWidgetOnLoad) {
    window.__openAnalyticsWidgetOnLoad = false;
    window.requestAnimationFrame(openWidget);
  }
}());
