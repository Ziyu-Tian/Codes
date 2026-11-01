/* Manual savings are dated observations; transfer links never increment them. */
function manualBalanceAt(snapshot, targetDate) {
  const rows = (snapshot.history || []).filter(row => row.as_of <= targetDate).sort((a, b) => a.as_of.localeCompare(b.as_of));
  return rows.length ? rows[rows.length - 1].balance : null;
}

let manualReturnToBalances = false;

function openManualAccount(key = '') {
  const account = manualAccounts.find(item => item.key === key);
  manualReturnToBalances = Boolean(account && $('#balanceDialog').open);
  if ($('#balanceDialog').open) $('#balanceDialog').close();
  const form = $('#manualAccountForm');
  form.reset();
  form.elements.key.value = key;
  form.elements.label.value = account?.label || '';
  form.elements.type.value = account?.type || 'cash_isa';
  form.elements.archived.checked = account?.archived || false;
  form.elements.identifier.placeholder = account?.identifier_hint ? `Saved ${account.identifier_hint}; leave blank to keep` : 'Sort code + account number (optional)';
  const today = new Date();
  form.elements.as_of.value = `${today.getFullYear()}-${String(today.getMonth() + 1).padStart(2, '0')}-${String(today.getDate()).padStart(2, '0')}`;
  $('#manualAccountError').textContent = '';
  $('#manualAccountTitle').textContent = account ? `Update ${account.label}` : 'Add new account';
  $('#manualHistorySection').hidden = !account;
  $('#manualArchiveField').hidden = !account;
  form.elements.balance.required = !account;
  $('#manualAccountSave').textContent = account ? 'Save changes' : 'Add account';
  $('#manualAccountCancel').textContent = manualReturnToBalances ? 'Back to balances' : 'Cancel';
  const history = balanceSnapshots[key]?.history || [];
  $('#manualBalanceHistory').replaceChildren();
  [...history].reverse().forEach(row => {
    const button = document.createElement('button');
    button.type = 'button';
    button.className = 'manual-history-row';
    button.textContent = `${row.as_of} · ${money(row.balance)}${row.note ? ' · ' + row.note : ''} — edit`;
    button.onclick = () => {
      form.elements.balance.value = row.balance;
      form.elements.as_of.value = row.as_of;
      form.elements.note.value = row.note || '';
    };
    $('#manualBalanceHistory').append(button);
  });
  if (!$('#manualAccountDialog').open) $('#manualAccountDialog').showModal();
}

function openManualTransfer(fingerprint) {
  const item = allTransactions.find(row => row.transaction_fingerprint === fingerprint);
  if (!item) return;
  const form = $('#manualTransferForm');
  form.reset();
  form.elements.fingerprint.value = fingerprint;
  $('#manualTransferDescription').textContent = `${item.date} · ${item.memo} · ${money(item.amount)}`;
  const select = form.elements.account_key;
  select.replaceChildren(new Option('External / remove savings link', ''));
  manualAccounts.forEach(account => select.add(new Option(account.label + (account.archived ? ' (archived)' : ''), account.key)));
  select.value = item.internal_reason === 'manual_account_transfer' ? item.linked_account_key : '';
  $('#manualTransferError').textContent = '';
  $('#manualTransferDialog').showModal();
}

$('#addManualAccount').onclick = () => openManualAccount();
$('#manualAccountDialog').addEventListener('close', () => {
  $('#manualAccountForm').elements.identifier.value = '';
  if (manualReturnToBalances) {
    manualReturnToBalances = false;
    openBalanceDialog(true);
  }
});
document.querySelectorAll('[data-close-manual]').forEach(button => {
  button.onclick = () => document.getElementById(button.dataset.closeManual).close();
});
$('#manualAccountForm').onsubmit = async event => {
  event.preventDefault();
  const form = event.currentTarget;
  const submit = form.querySelector('[type="submit"]');
  submit.disabled = true;
  try {
    const fields = Object.fromEntries(new FormData(form));
    fields.archived = form.elements.archived.checked;
    fields.clear_identifier = form.elements.clear_identifier.checked;
    if (!fields.key) delete fields.key;
    await request('/api/manual-accounts', { method: 'POST', body: JSON.stringify({ ...fields, dataset: $('#datasetFilter').value }) });
    form.elements.identifier.value = '';
    await loadDashboard();
    $('#manualAccountDialog').close();
  } catch (error) {
    $('#manualAccountError').textContent = error.message;
  } finally { submit.disabled = false; }
};
$('#manualTransferForm').onsubmit = async event => {
  event.preventDefault();
  const form = event.currentTarget;
  const submit = form.querySelector('[type="submit"]');
  submit.disabled = true;
  try {
    await request('/api/manual-transfers', { method: 'POST', body: JSON.stringify({
      dataset: $('#datasetFilter').value,
      fingerprint: form.elements.fingerprint.value,
      account_key: form.elements.account_key.value || null,
      remember: form.elements.remember.checked
    }) });
    $('#manualTransferDialog').close();
    await loadDashboard();
  } catch (error) {
    $('#manualTransferError').textContent = error.message;
  } finally { submit.disabled = false; }
};
