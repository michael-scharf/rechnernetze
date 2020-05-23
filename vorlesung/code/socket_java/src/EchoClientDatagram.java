import java.io.*;
import java.net.*;

public class EchoClientDatagram {

  public static void main(String[] args) {
    if (args.length != 2) {
      System.err.println("Usage: java EchoClientDatagram <hostname> <port>");
      System.exit(1);
    }

    String hostName = args[0];
    int portNumber = Integer.parseInt(args[1]);

    try {
      InetAddress serverAddress = InetAddress.getByName(hostName);

      DatagramSocket datagramSocket = new DatagramSocket();

      BufferedReader stdInput =
        new BufferedReader(new InputStreamReader(System.in));
      String inputLine;

      while ((inputLine = stdInput.readLine()) != null) {
        byte[] buffer = inputLine.getBytes();

        DatagramPacket requestPacket =
          new DatagramPacket(buffer, buffer.length, serverAddress, portNumber);
        datagramSocket.send(requestPacket);

        buffer = new byte[1024];

        DatagramPacket replyPacket = new DatagramPacket(buffer, buffer.length);
        datagramSocket.receive(replyPacket);

        String outputLine = new String(replyPacket.getData());
        System.out.println("Echo: " + outputLine);
      }
    } catch (UnknownHostException e) {
      System.err.println("Error: Unknown host " + e); System.exit(1);
    } catch (IOException e) {
      System.err.println("Error: No communication " + e); System.exit(1);
    }
  }

}
