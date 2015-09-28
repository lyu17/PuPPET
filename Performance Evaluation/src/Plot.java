import java.text.*;
import java.util.Vector;


public class Plot {
	
	private static DecimalFormat df = new DecimalFormat( "0.0000" );
	private static String path = "C:\\Users\\LI\\Desktop\\CPNs\\Experiments\\";
	private static Vector<String> mList  = new Vector<String>();
	private static Vector<String> nList  = new Vector<String>();
	private static Vector<String> dList  = new Vector<String>();
//	private static int[][] thresh = {{1385,1539},{1360,1511},{1651,1834},{1559,1732}};
	private static int[][] thresh = {{1200,1333},{1200,1333},{1200,1333},{1200,1333}};

	public static void main(String[] args) {
		
		mList.add("Jan");mList.add("Feb");mList.add("Mar");mList.add("Apr");
		nList.add("FCFS");nList.add("BLOCK");nList.add("WAIT");
		dList.add("DVFS1");dList.add("DVFS2");dList.add("DVFS3");
		
		for(int i=0; i<mList.size(); i++) {
			
			String month = mList.elementAt(i);
			double fcfsenergy = 0;
			double fixrate = 0;
			System.out.println(month + "\n");
			
//			System.out.println("Utilization");
			for(int j=0; j<nList.size(); j++) {
				String strategy = nList.elementAt(j);
				String filePath = path +  month + "\\output\\logfiles\\" + strategy;
				double util = Calculate.getUtil(filePath);
				System.out.print(df.format(util) + " " );
			}
			System.out.println();
			
//			System.out.println("Averge Wait Time");
			for(int j=0; j<nList.size(); j++) {
				String strategy = nList.elementAt(j);
				String filePath = path +  month + "\\output\\logfiles\\" + strategy;
				double awt = Calculate.getAWT(filePath);
				System.out.print(df.format(awt) + " " );
			}
			System.out.println();
			
//			System.out.println("EDP");
			for(int j=0; j<nList.size(); j++) {
				String strategy = nList.elementAt(j);
				String filePath = path +  month + "\\output\\logfiles\\" + strategy;
				double energy = Calculate.getEnergy(filePath);
				double ct = Calculate.getCT(filePath);
				double edp = energy*ct/100000000;
				if(j==0) {
					fcfsenergy = energy;
				}
				System.out.print(df.format(edp) + " " );
			}
			System.out.println("\n");
			
//			System.out.println("DVFS Utilization");
			for(int j=0; j<dList.size(); j++) {
				String strategy = dList.elementAt(j);
				String filePath = path +  month + "\\output\\logfiles\\" + strategy;
				double util = Calculate.getUtil(filePath);
				System.out.print(df.format(util) + " " );
			}
			System.out.println();
			
//			System.out.println("DVFS Averge Wait Time");
			for(int j=0; j<dList.size(); j++) {
				String strategy = dList.elementAt(j);
				String filePath = path +  month + "\\output\\logfiles\\" + strategy;
				double awt = Calculate.getAWT(filePath);
				System.out.print(df.format(awt) + " " );
			}
			System.out.println();
			
//			System.out.println("DVFS Energy");
			for(int j=0; j<dList.size(); j++) {
				String strategy = dList.elementAt(j);
				String filePath = path +  month + "\\output\\logfiles\\" + strategy;
				double energy = Calculate.getDVFSEnergy(thresh[i],filePath);
				double ct = Calculate.getCT(filePath);
				if(j==0) {
					fixrate = energy/fcfsenergy;
				}
				energy = energy / fixrate;
				double edp = energy*ct/100000000;
				System.out.print(df.format(edp) + " " );
			}
			System.out.println();
			
//			for(int k=0; k<dList.size(); k++) {
//				String strategy = dList.elementAt(k);
//				String filePath = path +  month + "\\output\\logfiles\\" + strategy;
//				double util = Calculate.getUtil(filePath);
//				double awt = Calculate.getAWT(filePath);
//				double energy = Calculate.getDVFSEnergy(thresh[i],filePath);
//				double ct = Calculate.getCT(filePath);
//				System.out.println(strategy + ": " + df.format(util) + " " + df.format(awt) + " " + df.format(energy)
//						 + " " + df.format(energy*ct/1000000));
//			}
			
			System.out.println();
			
		}
		
	}
	
}
	