import java.io.*;
import java.net.*;

public class TlsClient {

  public static void main(String[] args) {
    if (args.length != 1) {
      System.err.println("Usage: java TlsClient <uri>");
      System.exit(1);
    }

    try {
      URL url = new URL(args[0]);
     
      BufferedReader inReader = 
        new BufferedReader(new InputStreamReader(url.openStream()));

      String outputLine;
      while ((outputLine = inReader.readLine()) != null) {
        System.out.println(outputLine);
      }
      inReader.close();
    } catch (IOException e) {
      System.err.println("Error: Exception " + e); System.exit(1);
    }
  }

}
