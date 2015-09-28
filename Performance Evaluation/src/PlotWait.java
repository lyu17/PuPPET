import java.text.*;
import java.util.Vector;


public class PlotWait {
	
	private static DecimalFormat df = new DecimalFormat( "0.0000" );
	private static String path = "C:\\Users\\LI\\Desktop\\CPNs\\ExperimentsW\\";
	private static Vector<String> mList  = new Vector<String>();
	private static Vector<String> nList  = new Vector<String>();
	private static Vector<String> dList  = new Vector<String>();

	public static void main(String[] args) {
		
		mList.add("2");mList.add("5");mList.add("10");mList.add("20");mList.add("30");
		nList.add("2");nList.add("4");nList.add("6");nList.add("8");nList.add("10");
		
		for(int i=0; i<mList.size(); i++) {
			
			String qsize = mList.elementAt(i);
			double fcfsenergy = 0;
			double fixrate = 0;
			System.out.println(qsize + "\n");
			
//			System.out.println("Utilization");
			for(int j=0; j<nList.size(); j++) {
				String wt = nList.elementAt(j);
				String filePath = path +  qsize + "\\" + wt + "\\output\\logfiles\\WAIT";
				double util = Calculate.getUtil(filePath);
				System.out.print(df.format(util) + " " );
			}
			System.out.println();
			
//			System.out.println("Averge Wait Time");
			for(int j=0; j<nList.size(); j++) {
				String wt = nList.elementAt(j);
				String filePath = path +  qsize + "\\" + wt + "\\output\\logfiles\\WAIT";
				double awt = Calculate.getAWT(filePath);
				System.out.print(df.format(awt) + " " );
			}
			System.out.println();
			
//			System.out.println("EDP");
			for(int j=0; j<nList.size(); j++) {
				String wt = nList.elementAt(j);
				String filePath = path +  qsize + "\\" + wt + "\\output\\logfiles\\WAIT";
				double energy = Calculate.getEnergy(filePath);
				double ct = Calculate.getCT(filePath);
				double edp = energy*ct/100000000;
				if(j==0) {
					fcfsenergy = energy;
				}
				System.out.print(df.format(edp) + " " );
			}
			System.out.println("\n");
			
			System.out.println();
			
		}
		
	}
	
}
	