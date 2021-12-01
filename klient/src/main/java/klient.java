import java.io.*;
import java.net.*;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Scanner;

public class klient {
    //args[0] - nazwa hosta
    //args[1] - numer portu
    static int buffer_size;
    public static void main(String[] args) {

        String command;

        if (args.length < 2) {
            System.out.println("Podaj nazwe hosta i port");
            return;
        }

        try {
            // ustal adres serwera
            InetAddress addr = InetAddress.getByName(args[0]);
            // ustal port
            int port = Integer.parseInt(args[1]);
            // utworz gniazdo i od razu podlacz je do addr:port
            Socket socket = new Socket(addr, port);
            // strumienie
            DataOutputStream dos = new DataOutputStream(socket.getOutputStream());
            DataInputStream dis = new DataInputStream(socket.getInputStream());

            Scanner scanIn = new Scanner(System.in);
            recive(dis, 8, 2);  // Odebranie BUFFER_SIZE od serwera
            while (true) {
                // available stream to be read
                command = "path";
                // Wyslanie stringa
                dos.write(command.getBytes());

                // pobranie path z serwera
                recive(dis, buffer_size, 0);

                // available stream to be read
                //System.out.println("Enter command: ");
                if ((command = scanIn.nextLine()).isEmpty() || !(command.matches(".*\\w.*")))
                    continue;

                // wyslanie stringa
                dos.write(command.getBytes());

                if (command.equals("exit"))
                    break;

                // odpowiedz z serwera
                recive(dis, buffer_size, 1);
            }

            dos.write("break".getBytes()); // zamkniecie serwera
            dis.close();
            dos.close();
            socket.close(); // koniec rozmowy

        } catch (UnknownHostException ex) {
            System.out.println("Server not found: " + ex.getMessage());

        } catch (IOException ex) {
            System.out.println("I/O error: " + ex.getMessage());
        }
        System.out.println("Klient zakonczyl dzialanie");
    }

    public static void recive(DataInputStream dis, int buffer_size, int parametr) throws IOException {
        int length = 0;
        while ((length = dis.available()) == 0) ;
        // create buffer
        byte[] buf = new byte[length];
        // read the full data into the buffer
        dis.readFully(buf);
        Charset charset1 = StandardCharsets.UTF_8;
        String s = new String(buf, charset1).trim();

        if ((length == buffer_size) && !s.isEmpty() && parametr == 0) {
            System.out.printf(s + "> ");
        }
        if ((length == buffer_size) && !s.isEmpty() && parametr == 1) {
            System.out.println("Serwer> " + s);
        }
        if ((length == buffer_size) && !s.isEmpty() && parametr == 2) {
            klient.buffer_size=Integer.parseInt(s);
        }
    }
}