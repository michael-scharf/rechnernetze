import java.io.*;
import java.net.*;

public class EchoClient {

  public static void main(String[] args) {
    if (args.length != 2) {
      System.err.println("Usage: java EchoClient <hostname> <port>");
      System.exit(1);
    }

    String hostName = args[0];
    int portNumber = Integer.parseInt(args[1]);

    try {
      Socket echoSocket = new Socket(hostName, portNumber);

      PrintWriter outWriter =
        new PrintWriter(echoSocket.getOutputStream(), true);
      BufferedReader inReader =
        new BufferedReader(new InputStreamReader(echoSocket.getInputStream()));

      BufferedReader stdInput =
        new BufferedReader(new InputStreamReader(System.in));
      String inputLine;

      while ((inputLine = stdInput.readLine()) != null) {
        outWriter.println(inputLine);

        System.out.println("Echo: " + inReader.readLine());
      }
    } catch (UnknownHostException e) {
      System.err.println("Error: Unknown host " + e); System.exit(1);
    } catch (IOException e) {
      System.err.println("Error: No connection " + e); System.exit(1);
    }
  }

}
