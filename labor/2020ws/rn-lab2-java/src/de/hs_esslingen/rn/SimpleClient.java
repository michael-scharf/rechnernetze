package de.hs_esslingen.rn;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class SimpleClient {
  private static final String HOST_NAME = "localhost";
  private static final int PORT_NUMBER = 80;

  public static void main(final String[] args) {
    try (Socket simpleSocket = new Socket(HOST_NAME, PORT_NUMBER);
        PrintWriter outWriter = new PrintWriter(simpleSocket.getOutputStream(), true);
        BufferedReader inReader = new BufferedReader(new InputStreamReader(simpleSocket.getInputStream()));
        BufferedReader stdInput = new BufferedReader(new InputStreamReader(System.in));) {
      System.out.println("--- Enter request in format GET /param ...---");
      String requestLine = "GET /";
      System.out.print(requestLine);
      requestLine += stdInput.readLine() + "\r\n";

      outWriter.print(requestLine);
      outWriter.flush();
      final long totalsent = requestLine.length();

      String responseLine;
      long totalrecv = 0;
      while ((responseLine = inReader.readLine()) != null) {
        System.out.println(responseLine);
        totalrecv += responseLine.length() + 2; // for "\r\n"
        // Further actions could be added below
      }

      System.out.println();
      System.out.println("--- Statistics ---");
      System.out.println("Sent: " + totalsent + " bytes");
      System.out.println("Received: " + totalrecv + " bytes");
    } catch (final Exception e) {
      System.err.println("Client exception: " + e);
      System.exit(1);
    }
  }
}
