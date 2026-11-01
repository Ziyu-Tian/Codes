/* Run against the isolated fixture server only: PFD_UI_TEST_URL=http://127.0.0.1:3001 */
const assert = require('node:assert/strict');
const { chromium } = require('playwright');

(async () => {
  const url = process.env.PFD_UI_TEST_URL;
  assert.ok(url && new URL(url).port === '3001', 'Use the isolated fixture server on port 3001');
  const browser = await chromium.launch({ headless: true, executablePath: process.env.BROWSER_EXECUTABLE || '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome' });
  try {
    const page = await browser.newPage({ viewport: { width: 1440, height: 1000 } });
    page.setDefaultTimeout(10000);
    const errors = [];
    page.on('pageerror', error => errors.push(error.message));
    await page.goto(url, { waitUntil: 'networkidle' });
    await page.selectOption('#datasetFilter', 'personal');
    await page.waitForFunction(() => window.__dashboardDataReady);
    async function create(label, type, amount, identifier = '') {
      await page.click('#addManualAccount');
      assert.equal(await page.locator('#manualAccountTitle').innerText(), 'Add new account');
      assert.equal(await page.locator('#manualHistorySection').isVisible(), false);
      await page.fill('#manualAccountForm [name=label]', label);
      await page.selectOption('#manualAccountForm [name=type]', type);
      await page.fill('#manualAccountForm [name=balance]', amount);
      await page.fill('#manualAccountForm [name=as_of]', '2026-09-30');
      if (identifier) {
        await page.locator('#manualAccountForm details').first().locator('summary').click();
        await page.fill('#manualAccountForm [name=identifier]', identifier);
      }
      await page.click('#manualAccountForm [type=submit]');
      await page.locator('#manualAccountDialog').waitFor({ state: 'hidden' });
      await page.waitForFunction(() => window.__dashboardDataReady);
    }
    await create('Example ISA', 'cash_isa', '1000', '00-00-00 00000002');
    await create('Example Saver', 'saver', '2000');
    assert.equal(await page.locator('#totalDeposits').innerText(), '£3,000.00');
    assert.match(await page.locator('#accountPositions').innerText(), /2 transfers to reconcile/);
    const payload = await (await page.request.get(`${url}/api/transactions?dataset=personal`)).json();
    const isa = payload.accounts.find(a => a.label === 'Example ISA');
    const saver = payload.accounts.find(a => a.label === 'Example Saver');
    assert.equal(isa.identifier_hint, '•••• 0002');
    assert.equal(isa.identifier, undefined);
    assert.equal(payload.balances[isa.key].balance, 1000);
    assert.equal(await page.locator('#accountPositions [data-edit-manual]').count(), 0);
    await page.click('#balanceButton');
    await page.fill('#balanceFields .snapshot-balance', '555');
    await page.click(`#manualAccountList [data-edit-manual="${isa.key}"]`);
    assert.equal(await page.locator('#balanceDialog').isVisible(), false);
    await page.fill('#manualAccountForm [name=balance]', '1106');
    await page.fill('#manualAccountForm [name=as_of]', '2026-10-31');
    await page.click('#manualAccountForm [type=submit]');
    await page.locator('#manualAccountDialog').waitFor({ state: 'hidden' });
    await page.waitForFunction(() => window.__dashboardDataReady);
    await page.locator('#balanceDialog').waitFor({ state: 'visible' });
    assert.equal(await page.inputValue('#balanceFields .snapshot-balance'), '555');
    await page.click('#closeBalanceDialog');
    assert.equal(await page.locator('#totalDeposits').innerText(), '£3,106.00');
    assert.equal(await page.locator('.manual-pending').count(), 0);
    await page.click('#balanceButton');
    await page.click(`#manualAccountList [data-edit-manual="${isa.key}"]`);
    assert.equal(await page.locator('.manual-history-row').count(), 2);
    await page.locator('#manualAccountForm details').last().locator('summary').click();
    await page.locator('.manual-history-row').first().click();
    assert.equal(await page.inputValue('#manualAccountForm [name=balance]'), '1106');
    await page.locator('[data-close-manual=manualAccountDialog]').first().click();
    await page.locator('#balanceDialog').waitFor({ state: 'visible' });
    await page.click('#closeBalanceDialog');
    await page.selectOption('#chartMode', 'deposits');
    await page.selectOption('#accountFilter', isa.key);
    assert.match(await page.locator('#chart').innerText(), /Sept|Sep/);
    await page.selectOption('#accountFilter', 'all');
    await page.click('[data-view-target=transactions]');
    const transfer = payload.transactions.find(t => t.amount === -30);
    await page.click(`[data-link-transfer="${transfer.transaction_fingerprint}"]`);
    await page.selectOption('#manualTransferForm [name=account_key]', saver.key);
    await page.check('#manualTransferForm [name=remember]');
    await page.click('#manualTransferForm [type=submit]');
    await page.locator('#manualTransferDialog').waitFor({ state: 'hidden' });
    await page.waitForFunction(() => window.__dashboardDataReady);
    await page.click('[data-view-target=overview]');
    assert.equal(await page.locator('#spent').innerText(), '£20.00');
    assert.equal(await page.locator('#totalDeposits').innerText(), '£3,106.00');
    await page.screenshot({ path: '/tmp/pfd-manual-accounts-desktop.png', fullPage: true });
    await page.setViewportSize({ width: 375, height: 812 });
    await page.click('#balanceButton');
    await page.click(`#manualAccountList [data-edit-manual="${saver.key}"]`);
    const box = await page.locator('#manualAccountDialog').boundingBox();
    assert.ok(box.x >= 0 && box.x + box.width <= 375);
    await page.screenshot({ path: '/tmp/pfd-manual-accounts-mobile.png' });
    assert.deepEqual(errors, []);
    console.log('PASS: two accounts, masked identity, exact transfers, history, deposits, learned links, responsive dialog; no browser errors.');
  } finally {
    await browser.close();
  }
})().catch(error => { console.error(error); process.exit(1); });
