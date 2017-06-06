import java.io.*;
import java.net.*;

public class Server {
  private static int PORT = 8100;

  private Server() {
    try (ServerSocket server = new ServerSocket(PORT);) {
      System.out.println("listening... port:" + PORT);
      while (true) {
        try (Socket socket = server.accept();) {
          accept(socket);
        } catch(Exception e) {
          e.printStackTrace();
        }
      }
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  private void accept(Socket socket) throws IOException {
    System.out.println("accepted");

    BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
    char[] buf = new char[2048];
    while(br.read(buf) != -1) {
      System.out.println(buf);
    }
  }

  public static void main(String args[]) {
    System.out.println("hoge");
    Server server = new Server();
  }
}
