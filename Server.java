import java.io.*;
import java.net.*;
import java.util.*;

public class Server {
  private static int PORT = 8100;
  Task task = new Task();

  public static void main(String args[]) {
    Server server = new Server();
  }

  private Server() {
    try (ServerSocket server = new ServerSocket(PORT);) {
      System.out.println("listening... port:" + PORT);
      while (true) {
        try (Socket socket = server.accept();) {
          accept(socket);
        }
      }
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  private void accept(Socket socket) throws IOException, ClassNotFoundException {
    System.out.println(">>> accepted");

    DataInputStream is = new DataInputStream(socket.getInputStream());

    String command = "";
    char c;
    while ((c = (char)is.readByte()) != '\0') {
      command += c;
    }
    System.out.println("command: " + command);

    Iterator<String> commands = Arrays.asList(command.split(" ")).iterator();
    String command1 = commands.next();
    System.out.println("command 1 : " + command1);

    if (command1.equals("fetch")) {
      ObjectOutputStream oos = new ObjectOutputStream(socket.getOutputStream());
      oos.writeObject(task);
    } else if (command1.equals("call")) {
      int len = is.readInt();
      System.out.println("lenth: " + len);
      byte[] bytes = new byte[len];
      for (int i = 0; i < len; i++) {
        byte b = is.readByte();
        System.out.print(String.format("%02x", b & 0xFF));
        System.out.print(i % 10 == 9 ? "\n" : " ");
        bytes[i] = (byte)b;
      }
      System.out.print("\n");

      ObjectInputStream ois = new ObjectInputStream(new ByteArrayInputStream(bytes));
      Person person = (Person)ois.readObject();
      ois.close();

      task.hello(person);

    } else {
      System.out.println("invalid command");
    }

    System.out.println("<<< end");
  }
}
