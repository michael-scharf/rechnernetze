import java.io.*;
import java.net.*;

public class EchoServerDatagram {

  public static void main(String[] args) {
    if (args.length != 1) {
      System.err.println("Usage: java EchoServerDatagram <port>");
      System.exit(1);
    }

    int portNumber = Integer.parseInt(args[0]);

    try {
      DatagramSocket datagramSocket = new DatagramSocket(portNumber);

      while (true) {
        byte[] buffer = new byte[1024];

        DatagramPacket requestPacket =
          new DatagramPacket(buffer, buffer.length);
        datagramSocket.receive(requestPacket);

        buffer = requestPacket.getData();

        DatagramPacket replyPacket =
          new DatagramPacket(buffer, buffer.length, requestPacket.getAddress(),
                             requestPacket.getPort());
        datagramSocket.send(replyPacket);
      }
    } catch (IOException e) {
      System.err.println("Error: Exception " + e); System.exit(1);
    }
  }

}
