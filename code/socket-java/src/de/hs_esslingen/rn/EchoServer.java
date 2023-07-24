package de.hs_esslingen.rn;

import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.PrintWriter;
import java.io.IOException;
import java.net.Socket;
import java.net.ServerSocket;

public class EchoServer {

  public static void main(String[] args) {
    if (args.length != 1) {
      System.err.println("Usage: java EchoServer <port>");
      System.exit(1);
    }

    int portNumber = Integer.parseInt(args[0]);

    try {
      ServerSocket serverSocket = new ServerSocket(portNumber);
      Socket clientSocket = serverSocket.accept();

      PrintWriter outWriter =
        new PrintWriter(clientSocket.getOutputStream(), true);
      BufferedReader inReader =
        new BufferedReader(
          new InputStreamReader(clientSocket.getInputStream()));

      String requestLine;

      while ((requestLine = inReader.readLine()) != null) {
        outWriter.println(requestLine);
      }
    } catch (IOException e) {
      System.err.println("Error: Exception " + e); System.exit(1);
    }
  }

}
