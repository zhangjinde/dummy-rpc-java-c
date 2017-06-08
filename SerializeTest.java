import java.io.*;
import java.util.*;

class SerializeTest {
  public static void main(String[] args) throws Exception {
    File file = new File("./person.bin");
    file.createNewFile();
    Person person = new Person();
    ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(file));
    oos.writeObject(person);
    oos.close();
  }
}
