source: http://www.securityfocus.com/bid/32909/info

GNU Classpath is prone to a weakness that may result in weaker cryptographic security because its psuedo-random number generator (PRNG) lacks entropy.

Attackers may leverage this issue to obtain sensitive information that can lead to further attacks.

Classpath 0.97.2 is vulnerable; other versions may also be affected. 

import gnu.java.security.util.PRNG;

class PRNGTest {
   public static void main(String args[])
      {
      long t = System.currentTimeMillis();

      System.out.println("Time in ms is " + t);

      PRNG prng = PRNG.getInstance();

      byte buffer[] = new byte[40];

      prng.nextBytes(buffer, 0, buffer.length);

      for(int i = 0; i != buffer.length; ++i)
         {
         // There must be an easier way to do this (right?)
         int intval = buffer[i];

         if(intval <= 0)
            intval += 256;

         String s = Integer.toHexString(intval);

         if(s.length() == 1)
            s = "0" + s;

         System.out.print(s);
         }
      System.out.println("");
      }
   };