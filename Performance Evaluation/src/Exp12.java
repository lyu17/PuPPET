import java.text.*;
import java.util.Vector;


public class Exp12 {
	
	private static DecimalFormat df = new DecimalFormat( "0.0000" );
	private static String path = "C:\\Users\\LI\\Desktop\\CPNs3\\WFP\\SCHVP\\";
	private static Vector<String> mList  = new Vector<String>();
	private static Vector<String> nList  = new Vector<String>();
	
	private static Vector<Double> Jan_su  = new Vector<Double>();
	private static Vector<Double> Jan_aw  = new Vector<Double>();
	private static Vector<Double> Jan_ed  = new Vector<Double>();
	private static Vector<Double> Feb_su  = new Vector<Double>();
	private static Vector<Double> Feb_aw  = new Vector<Double>();
	private static Vector<Double> Feb_ed  = new Vector<Double>();
	private static Vector<Double> Mar_su  = new Vector<Double>();
	private static Vector<Double> Mar_aw  = new Vector<Double>();
	private static Vector<Double> Mar_ed  = new Vector<Double>();
	private static Vector<Double> Apr_su  = new Vector<Double>();
	private static Vector<Double> Apr_aw  = new Vector<Double>();
	private static Vector<Double> Apr_ed  = new Vector<Double>();

	public static void main(String[] args) {
		
		mList.add("m1");mList.add("m2");mList.add("m3");mList.add("m4");
		nList.add("100");nList.add("80");nList.add("60");nList.add("40");
		
		for(int i=0; i<mList.size(); i++) {
			
			String month = mList.elementAt(i);
			System.out.println(month + "\n");
			
//			System.out.println("Utilization");
			for(int j=0; j<nList.size(); j++) {
				String strategy = nList.elementAt(j);
				String filePath = path +  month + "\\" + strategy + "\\output\\logfiles\\WAIT";
				double util = Calculate.getUtil(filePath);
				System.out.print(df.format(util) + " " );
				if(i==0) {
					Jan_su.addElement(util);
				}
				else if(i==1) {
					Feb_su.addElement(util);
				}
				else if(i==2) {
					Mar_su.addElement(util);
				}
				else if(i==3) {
					Apr_su.addElement(util);
				}
			}
			System.out.println();
			
//			System.out.println("Averge Wait Time");
			for(int j=0; j<nList.size(); j++) {
				String strategy = nList.elementAt(j);
				String filePath = path +  month + "\\" + strategy + "\\output\\logfiles\\WAIT";
				double awt = Calculate.getAWT(filePath);
				System.out.print(df.format(awt) + " " );
				if(i==0) {
					Jan_aw.addElement(awt);
				}
				else if(i==1) {
					Feb_aw.addElement(awt);
				}
				else if(i==2) {
					Mar_aw.addElement(awt);
				}
				else if(i==3) {
					Apr_aw.addElement(awt);
				}
			}
			System.out.println();
			
//			System.out.println("EDP");
			for(int j=0; j<nList.size(); j++) {
				String strategy = nList.elementAt(j);
				String filePath = path +  month + "\\" + strategy + "\\output\\logfiles\\WAIT";
				double energy = Calculate.getEnergy(filePath);
				double ct = Calculate.getCT(filePath);
				double edp = energy*ct/100000;
				System.out.print(df.format(edp) + " " );
				if(i==0) {
					Jan_ed.addElement(edp);
				}
				else if(i==1) {
					Feb_ed.addElement(edp);
				}
				else if(i==2) {
					Mar_ed.addElement(edp);
				}
				else if(i==3) {
					Apr_ed.addElement(edp);
				}
			}
			System.out.println("\n");
			
		}
		
		/* ********************************************************************************************************/
		
		System.out.println("Utilization");
		for(int i=0; i<4; i++) {
			System.out.print(df.format(Jan_su.elementAt(i))+" ");
		}
		System.out.println();
		for(int i=0; i<4; i++) {
			System.out.print(df.format(Feb_su.elementAt(i))+" ");
		}
		System.out.println();
		for(int i=0; i<4; i++) {
			System.out.print(df.format(Mar_su.elementAt(i))+" ");
		}
		System.out.println();
		for(int i=0; i<4; i++) {
			System.out.print(df.format(Apr_su.elementAt(i))+" ");
		}
		System.out.println("\n");
		
		/* ********************************************************************************************************/
		
		System.out.println("AWT");
		for(int i=0; i<4; i++) {
			System.out.print(df.format(Jan_aw.elementAt(i))+" ");
		}
		System.out.println();
		for(int i=0; i<4; i++) {
			System.out.print(df.format(Feb_aw.elementAt(i))+" ");
		}
		System.out.println();
		for(int i=0; i<4; i++) {
			System.out.print(df.format(Mar_aw.elementAt(i))+" ");
		}
		System.out.println();
		for(int i=0; i<4; i++) {
			System.out.print(df.format(Apr_aw.elementAt(i))+" ");
		}
		System.out.println("\n");
		
		/* ********************************************************************************************************/
		
		double base1 = Jan_ed.elementAt(0);
		double base2 = Feb_ed.elementAt(0);
		double base3 = Mar_ed.elementAt(0);
		double base4 = Apr_ed.elementAt(0);
		System.out.println("EDP");
		for(int i=0; i<4; i++) {
			System.out.print(df.format(Jan_ed.elementAt(i)/base1)+" ");
		}
		System.out.println();
		for(int i=0; i<4; i++) {
			System.out.print(df.format(Feb_ed.elementAt(i)/base2)+" ");
		}
		System.out.println();
		for(int i=0; i<4; i++) {
			System.out.print(df.format(Mar_ed.elementAt(i)/base3)+" ");
		}
		System.out.println();
		for(int i=0; i<4; i++) {
			System.out.print(df.format(Apr_ed.elementAt(i)/base4)+" ");
		}
		System.out.println("\n");
		
		/* ********************************************************************************************************/
	}
	
}
	