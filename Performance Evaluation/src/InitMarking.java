import java.sql.*;
//import java.text.*;
//import java.util.*;

public class InitMarking {
	
//	private static DecimalFormat df = new DecimalFormat( "0.00" );
	
	private static Statement stat = null;
	private static ResultSet res = null;
	private static Connection conn = null;
	private static String query = "";
	
	public static void main(String[] args) {
		
		int numOfDay = 1;
		int interval = 3600;
	
		
		try {
			
			conn = DB.connect();
			stat = conn.createStatement();
		
			for(int i=0; i<numOfDay; i++) {
				
				System.out.println("Day: " + (i+1));
			
				
				int minTime = 1357020000 + i * 3600 * 24;
				int maxTime = 1357020000 + (i+1) * 3600 * 24;
			
//				int minTime = 1357020000 + 3600 * 24 * numOfDay;
//				int maxTime = 1357020000 + 3600 * 24  * (numOfDay+1);
				
				query = "select starttime_unixtime,duration,input_power,num_racks from analyzed_TotalPower where" + 
				        " starttime_unixtime>=" + minTime + 
						" and starttime_unixtime<" + maxTime + 
//						" and input_power > 0 and duration > " + interval +
						" order by starttime";
				
				res = stat.executeQuery(query);
				
				int k = 0;
				
				while(res.next())  {
					
					int starttime = res.getInt(1) - minTime;
					int runtime = res.getInt(2);
					double power = res.getInt(3);
					int rack = res.getInt(4);
					
					int pow = 0;
					if(power/rack/runtime/10-(int)(power/rack/runtime/10)>0.5) {
						pow = (int)(power/rack/runtime/10)+1;
					}
					else {
						pow = (int)(power/rack/runtime/10);
					}
					
					if(starttime%interval>interval/2) {
						starttime = starttime/interval + 1;
					}
					else {
						starttime = starttime/interval;
					}
					
					if(runtime%interval>interval/2) {
						runtime = runtime/interval + 1;
					}
					else {
						runtime = runtime/interval;
					}
					
					System.out.println("1`("+rack+","+runtime+","+pow+",1.0)@+"+starttime+"++");
					
					k++;
					
				}
				
				System.out.println(k);
				
			}
		
		}  catch (Exception e) {
		
			e.printStackTrace();
		
		}
		
	}
	
	
}