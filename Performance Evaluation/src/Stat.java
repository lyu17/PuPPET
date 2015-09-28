import java.sql.*;
import java.text.DecimalFormat;

public class Stat {
	
	private static DecimalFormat df = new DecimalFormat( "0.00" );
	
	private static Statement stat = null;
	private static ResultSet res = null;
	private static Connection conn = null;
	private static String query = "";

	public static void main(String[] args) {
		
		int interval = 60;
	
		String[] month = new String[] {"Jan","Feb","Mar","Apr"};
		String[] ranks = new String[] {"1","2","4","8","16","24","32","48"};
		
		try {
			
			conn = DB.connect();
			stat = conn.createStatement();
		
//			for(int i=0; i<month.length; i++) {
			for(int i=0; i<1; i++) {
				
				System.out.println(month[i] + ":");
				
//				int minTime = 1357020000 + i * 30 * 3600 * 24;
//				int maxTime = 1357020000 + (i+1) * 30 * 3600 * 24;
				
				int minTime = 1357020000 + i * 120 * 3600 * 24;
				int maxTime = 1357020000 + (i+1) * 120 * 3600 * 24;
				
//				double totalNum = 0;
//				double totalPow = 0;
//				double totalRun = 0;
				
				for(int j=0; j<ranks.length; j++) {
					
					double totalNum = 0;
					double totalPow = 0;
					
					query = "select starttime_unixtime,duration,input_power,num_racks from analyzed_TotalPower where" + 
				            " starttime_unixtime>=" + minTime + 
						    " and starttime_unixtime<" + maxTime + 
						    " and input_power > 0 and duration > " + interval +
						    " and num_racks = " + ranks[j] +
						    " order by starttime";
					
					res = stat.executeQuery(query);
					
					int num = 0;
				
					while(res.next())  {
						
//						int starttime = res.getInt(1) - minTime;
						int runtime = res.getInt(2);
						int pow = res.getInt(3);
						int rack = res.getInt(4);
						
						totalNum += 1;
//						totalRun += runtime;
						totalPow += pow / runtime;
						num += 1;
						
					}
					
//					System.out.print(num+" ");
					System.out.println("Power: " + df.format(totalPow/totalNum));
					
				}
				
//				System.out.println("\nLamda: " + df.format((3600/interval)*24*30/totalNum));
//				System.out.println("RunTime: " + df.format(totalRun/totalNum/interval));
//				System.out.println("Power: " + df.format(totalPow/totalNum));
//				System.out.println(totalNum);
			}
		
		}  catch (Exception e) {
		
			e.printStackTrace();
		
		}
		
//		System.out.println(maxRack+" "+maxRT+" "+maxPP+" "+maxST);
		
	}
	
	
}