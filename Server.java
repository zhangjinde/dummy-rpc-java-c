import java.io.*;
import java.net.*;
import java.util.*;
import java.lang.Class;
import java.lang.reflect.Method;

public class Server {
  private static int PORT = 8100;

  private static HashMap<String, Object> services;

  public static void main(String args[]) {
    services = new HashMap<String, Object>();
    services.put("Task", new Task());
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
    System.out.println("\n>>> accepted");

    DataInputStream dis = new DataInputStream(socket.getInputStream());

    String command = "";
    char c;
    while ((c = (char)dis.readByte()) != '\0') {
      command += c;
    }
    System.out.println("command: " + command);

    Iterator<String> commands = Arrays.asList(command.split(" ")).iterator();
    String command1 = commands.next();

    if (command1.equals("fetch")) {
      ObjectOutputStream oos = new ObjectOutputStream(socket.getOutputStream());
      fetch(oos, commands.next());
    } else if (command1.equals("call")) {
      call(dis, commands.next(), commands.next());
    } else {
      System.out.println("invalid command");
    }

    System.out.println("<<< end");
  }

  private void fetch(ObjectOutputStream oos, String serviceName) throws IOException {
    System.out.println("[fetch] " + serviceName);
    Object service = services.get(serviceName);
    oos.writeObject(service);
  }

  private void call(DataInputStream dis, String serviceName, String methodName) throws IOException {
    System.out.println("[call] " + serviceName + " " + methodName);

    int len = dis.readInt();
    System.out.println("args bytes lenth: " + len);
    byte[] bytes = new byte[len];

    System.out.println("hexdump:");
    for (int i = 0; i < len; i++) {
      byte b = dis.readByte();
      System.out.print(String.format("%02x", b & 0xFF));
      System.out.print(i % 10 == 9 ? "\n" : " ");
      bytes[i] = (byte)b;
    }
    System.out.print("\n");

    System.out.println("call:");
    try (ObjectInputStream ois = new ObjectInputStream(new ByteArrayInputStream(bytes));) {
      Object arg1 = ois.readObject();
      Object service = services.get(serviceName);
      Class<?> clazz = Class.forName(serviceName);
      Method method = clazz.getMethod(methodName, arg1.getClass());
      method.invoke(service, arg1);
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
}
