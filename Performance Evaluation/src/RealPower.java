import java.sql.*;
import java.text.*;

public class RealPower {
	
	private static DecimalFormat df = new DecimalFormat( "0.00" );
	
	private static Statement stat = null;
	private static ResultSet res = null;
	private static Connection conn = null;
	private static String query = "";
	
	public static void main(String[] args) {
		
		int numOfDay = 1;
		int interval = 3600*24;
	
		
		try {
			
			conn = DB.connect();
			stat = conn.createStatement();
			
			int start_time = 1357020000;
			int end_time = 1357020000 + 3600 * 24 * numOfDay;
		
			int k = 1;
			int timeIdx = 0;
			double old_power = 0;
			
			while(start_time+interval*k<=end_time) {
				
				double acc_power = 0;
				
				timeIdx = start_time + interval * k;
				query = "select starttime_unixtime,endtime_unixtime,duration,input_power,num_racks FROM analyzed_TotalPower" +
						" where starttime_unixtime>=" + start_time +
						" order by starttime";
				res = stat.executeQuery(query);
				
				int count = 0;
				
				while (res.next()) {
					
					count++;
					int st = res.getInt(1);
					int et = res.getInt(2);
//					int rt = res.getInt(3);
					double power = res.getDouble(4);
					
//     				System.out.println(timeIdx);
					
					if(st<timeIdx) {
						if(et<=timeIdx) {
							acc_power += power;
						}
						else if(et>timeIdx) {
							acc_power += power*(double)(timeIdx-st)/(et-st);;
						}
					}
					
				}
				System.out.println(count);
				System.out.println(k+": "+df.format(acc_power-old_power)+" "+df.format(acc_power));
				old_power = acc_power;
				
				k++;
				
			}
		
		}  catch (Exception e) {
		
			e.printStackTrace();
		
		}
		
//		System.out.println(maxRack+" "+maxRT+" "+maxPP+" "+maxST);
		
	}
	
	
}