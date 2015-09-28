import java.text.DecimalFormat;


public class TestArrivalRate {
	
	private static DecimalFormat df = new DecimalFormat( "0.0000" );
	
	private static String path = "C:\\Users\\LI\\Desktop\\CPNs\\Test\\output\\logfiles\\";
	
	public static void main(String[] args) {
		
		System.out.println("FCFS");
		
		int interval = 3600;
		
		int[][] FCFS_C = IOScanner.getData(path+"FCFS_C.log");

		double talUtil = 0;
//		int endTime = FCFS_C[FCFS_C.length-1][0];
		int endTime = 24*3600/interval;
		int strTime = FCFS_C[0][0];
			
		for(int i=0; i<FCFS_C.length-1; i++) {
			if(FCFS_C[i][0]!=FCFS_C[i+1][0]) {
				talUtil += (FCFS_C[i+1][0]-FCFS_C[i][0]) * (48-FCFS_C[i][1]);
//				System.out.println((FCFS_C_Data[i+1][0]-FCFS_C_Data[i][0]) * (48-FCFS_C_Data[i][1]));
			}
		}
	
		System.out.println("System Utilization: " + df.format(talUtil/endTime/48));
		
		int[][] FCFS_Submit = IOScanner.getData(path+"FCFS_Submit.log");
		int[][] FCFS_Run = IOScanner.getData(path+"FCFS_Run.log");
		
		double avgWT = 0;
			
		for(int i=0; i<FCFS_Submit.length; i++) {
			avgWT += FCFS_Run[i][0] - FCFS_Submit[i][0];
		}
	
		System.out.println("Average Time: " + df.format(avgWT/FCFS_Submit.length));
		
		int[][] FCFS_Release = IOScanner.getData(path+"FCFS_Release.log");
		
		double maxPow = 0;
		double accPow = 0;
		double curPow = 0;
		double talPow = 0;
		for(int i=0; i<=endTime; i++) {
			talPow += curPow;
			for(int j=0; j<FCFS_Run.length; j++) {
				if(FCFS_Run[j][0]==i) {
					curPow += FCFS_Run[j][1];
				}
				if(FCFS_Release[j][0]==i) {
					curPow -= FCFS_Release[j][1];
				}
			}
			if(curPow>maxPow) {
				maxPow = curPow;
			}
			accPow += curPow * interval * 10;
//			System.out.println(i+"accPow: " + df.format(accPow));
		}
	
		System.out.println("Energy Consumption: " + df.format(accPow));
		System.out.println("EDP: " + df.format(talPow*(endTime-strTime)));
		
		System.out.println("maxPow: " + df.format(maxPow));
		
	}
	
}
	