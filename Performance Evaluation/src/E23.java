import java.text.*;
import java.util.Vector;


public class E23 {
	
	private static DecimalFormat df = new DecimalFormat( "0.0000" );
	private static String path = "C:\\Users\\LI\\Desktop\\CPNs3\\WFP\\SCH\\";
	private static Vector<String> mList  = new Vector<String>();
	private static Vector<Integer> IList = new Vector<Integer>();
	
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
		IList.add(0);IList.add(10);IList.add(20);IList.add(30);IList.add(40);IList.add(50);
		
		for(int i=0; i<mList.size(); i++) {
			
			String month = mList.elementAt(i);
			System.out.println(month + "\n");
			
//			System.out.println("EDP");
			for(int j=0; j<IList.size(); j++) {
				int prate = IList.elementAt(j);
				String strategy = "FCFS";
				String filePath = path +  month + "\\output\\logfiles\\" + strategy;
				double energy = CalculateIDLE.getEnergy(filePath,prate);
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
		
		double base1 = Jan_ed.elementAt(0);
		double base2 = Feb_ed.elementAt(0);
		double base3 = Mar_ed.elementAt(0);
		double base4 = Apr_ed.elementAt(0);
		System.out.println("EDP");
		for(int i=0; i<6; i++) {
//			System.out.print(df.format(Jan_ed.elementAt(i)/base1)+" ");
			System.out.print(df.format(Jan_ed.elementAt(i))+" ");
		}
		System.out.println();
		for(int i=0; i<6; i++) {
//			System.out.print(df.format(Feb_ed.elementAt(i)/base2)+" ");
			System.out.print(df.format(Feb_ed.elementAt(i))+" ");
		}
		System.out.println();
		for(int i=0; i<6; i++) {
//			System.out.print(df.format(Mar_ed.elementAt(i)/base3)+" ");
			System.out.print(df.format(Mar_ed.elementAt(i))+" ");
		}
		System.out.println();
		for(int i=0; i<6; i++) {
//			System.out.print(df.format(Apr_ed.elementAt(i)/base4)+" ");
			System.out.print(df.format(Apr_ed.elementAt(i))+" ");
		}
		System.out.println("\n");
		
		/* ********************************************************************************************************/
	}
	
}
	