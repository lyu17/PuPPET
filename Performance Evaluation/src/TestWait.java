import java.text.DecimalFormat;


public class TestWait {
	
	private static DecimalFormat df = new DecimalFormat( "0.0000" );
	
	private static String path = "C:\\Users\\LI\\Desktop\\CPNs\\Test\\output\\logfiles\\";
	
	public static void main(String[] args) {
		
		System.out.println("WAIT");
		
		int[][] WAIT_C = IOScanner.getData(path+"WAIT_C.log");

		double talUtil = 0;
		int endTime = WAIT_C[WAIT_C.length-1][0];
		int strTime = WAIT_C[0][0];
			
		for(int i=0; i<WAIT_C.length-1; i++) {
			if(WAIT_C[i][0]!=WAIT_C[i+1][0]) {
				talUtil += (WAIT_C[i+1][0]-WAIT_C[i][0]) * (48-WAIT_C[i][1]);
//				System.out.println((WAIT_C_Data[i+1][0]-WAIT_C_Data[i][0]) * (48-WAIT_C_Data[i][1]));
			}
		}
	
		System.out.println("System Utilization: " + df.format(talUtil/endTime/48));
		
		int[][] WAIT_Submit = IOScanner.getData(path+"WAIT_Submit.log");
		int[][] WAIT_Run = IOScanner.getData(path+"WAIT_Run.log");
		
		double avgWT = 0;
			
		for(int i=0; i<WAIT_Submit.length; i++) {
			avgWT += WAIT_Run[i][0] - WAIT_Submit[i][0];
		}
	
		System.out.println("Average Time: " + df.format(avgWT/WAIT_Submit.length));
		
		int[][] WAIT_Release = IOScanner.getData(path+"WAIT_Release.log");
		
		double maxPow = 0;
		double accPow = 0;
		double curPow = 0;
		double talPow = 0;
		for(int i=0; i<=endTime; i++) {
			talPow += curPow;
			for(int j=0; j<WAIT_Run.length; j++) {
				if(WAIT_Run[j][0]==i) {
					curPow += WAIT_Run[j][1];
				}
				if(WAIT_Release[j][0]==i) {
					curPow -= WAIT_Release[j][1];
				}
			}
			if(curPow>maxPow) {
				maxPow = curPow;
			}
			accPow += curPow * 60 * 10;
//			System.out.println(i+"accPow: " + df.format(accPow));
		}
	
		System.out.println("Energy Consumption: " + df.format(talPow));
		System.out.println("EDP: " + df.format(talPow*(endTime-strTime)));
		
		System.out.println("maxPow: " + df.format(maxPow));
		
	}
	
}
	