import java.text.*;
import java.util.Vector;


public class Validate {
	
	private static DecimalFormat df = new DecimalFormat( "0.0000" );
	private static String path = "/Users/li/Desktop/Validation";
	private static Vector<String> mList  = new Vector<String>();
	private static Vector<String> nList  = new Vector<String>();

	public static void main(String[] args) {
		mList.add("m1");
		mList.add("m2");
		mList.add("m3");
		mList.add("m4");
		nList.add("FCFS");
		
		for(int i=0; i<mList.size(); i++) {
			String month = mList.elementAt(i);
			System.out.println(month + "\n");
			String filePath = path +  month + "\\output\\logfiles\\FCFS";
			double util = Calculate.getUtil(filePath);
			System.out.println();
		}
	}
	
}
	