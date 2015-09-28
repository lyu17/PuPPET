import java.sql.*;

public class DB{
	
	private static String url = "jdbc:mysql://216.47.152.43:3306/";
	private static String dbName = "ANLBGQ";
	private static String driver = "com.mysql.jdbc.Driver";
	private static String userName = "li";
	private static String password = "5j966UDv";

	public static Connection connect() {	
		Connection conn = null;
		try {
			Class.forName(driver).newInstance();
			conn = DriverManager.getConnection(url+dbName,userName,password);
			System.out.println("Connected to the database ...\n");		
		} catch (Exception e) {	
			e.printStackTrace();
		}
		return conn;
	}
	
	public static void close(Connection conn) {	
		try {
			conn.close();
			System.out.println("\nDisconnected from the database ...\n");		
		} catch (Exception e) {	
			e.printStackTrace();
		}
	}
	
} 