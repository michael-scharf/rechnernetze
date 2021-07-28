package de.hs_esslingen.rn;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.io.IOException;
import java.net.Socket;
import java.net.ServerSocket;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class SimpleServer {
  private static final int PORT_NUMBER = 42001;

  private static final String RESPONSE_PREFIX = "HTTP/1.0 200 OK\r\nServer: Rechnernetze SimpleServer\r\nContent-type: text/plain\r\n\r\n";

  private static final int RESPONSE_LEN = 10000000;

  private static final int RESPONSE_LINE = 80;
  private static final int RESPONSE_MARK = 10;
  private static final int RESPONSE_ALIGN = 13;

  public static void main(final String[] args) {
    new SimpleServer();
  }

  public SimpleServer() {
    try (ServerSocket serverSocket = new ServerSocket(PORT_NUMBER);) {
      while (true) {
        final Socket clientSocket = serverSocket.accept();
        final ClientThread clientThread = new ClientThread(clientSocket);
        clientThread.start();
      }
    } catch (final IOException e) {
      System.err.println("Server exception: " + e);
    }
  }

  private class ClientThread extends Thread {
    private final Socket clientSocket;

    ClientThread(final Socket socket) {
      clientSocket = socket;
    }

    public void run() {
      try (PrintWriter outWriter = new PrintWriter(clientSocket.getOutputStream(), true);
          BufferedReader inReader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));) {
        final String request = inReader.readLine();
        String response = "";

        int length = 0;
        int iterations = 1;
        int delay = 0;
        boolean valid = false;

        final Pattern pattern = Pattern.compile("GET\\s/(\\d+)(\\?(\\d+)(\\+(\\d+))?)?");
        final Matcher matcher = pattern.matcher(request);
        if (matcher.find()) {
          if (matcher.group(1) != null)
            length = Integer.parseInt(matcher.group(1));
          if (matcher.group(3) != null)
            iterations = Integer.parseInt(matcher.group(3));
          if (matcher.group(5) != null)
            delay = Integer.parseInt(matcher.group(5));

          if (length > 0 && length <= RESPONSE_LEN && iterations > 0 && delay >= 0) {
            valid = true;
          } else {
            iterations = 1;
            delay = 0;
            response = "HTTP/1.0 400 Bad request\r\n\r\nInvalid number\r\n";
          }
        } else {
          response = "HTTP/1.0 400 Bad request\r\n\r\nInvalid syntax\r\n";
        }

        for (int i = 0; i < iterations; i++) {
          if (valid)
            response = createResponse(length, (i == 0) ? true : false);

          outWriter.print(response);
          outWriter.flush();

          if ((delay > 0) && (i < (iterations - 1)))
            Thread.sleep(delay);
        }
      } catch (final Exception e) {
        System.err.println("Server thread exception: " + e);
      }
    }
  }

  private static String createResponse(final int responseLength, final boolean usePrefix) {
    int offset = 0;
    if (usePrefix) {
      offset = RESPONSE_PREFIX.length();
    }
    int length = responseLength;
    if (length < (offset + RESPONSE_ALIGN))
      length = offset + RESPONSE_ALIGN;
    if (length > RESPONSE_LEN)
      length = RESPONSE_LEN;

    final StringBuilder sb = new StringBuilder(length);
    if (usePrefix) {
      sb.append(RESPONSE_PREFIX);
    }

    for (int i = offset; i < length; i++) {
      if (((i + 1) % RESPONSE_MARK) == 0) {
        sb.append('_');
      } else {
        sb.append(".");
      }
      if (((i + 1 - offset) % RESPONSE_LINE) == 0) {
        sb.setCharAt(i - 1, '\r');
        sb.setCharAt(i, '\n');
      }
    }

    if (((length - offset - 1) % RESPONSE_LINE) < RESPONSE_ALIGN) {
      final String summary = String.format("\r\n%010d!\r\n", length);
      sb.replace(length - RESPONSE_ALIGN - 2, length - RESPONSE_ALIGN - 2 + summary.length(), summary);
    } else {
      final String summary = String.format("%010d!\r\n", length);
      sb.replace(length - RESPONSE_ALIGN, length - RESPONSE_ALIGN + summary.length(), summary);
    }

    return sb.toString();
  }

}
