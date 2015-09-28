import java.text.*;

public class Statistic {
	
	private static DecimalFormat df = new DecimalFormat( "0.0000" );
	
	private static String path = "C:\\Users\\LI\\Desktop\\CPNs\\Experiments\\Jan\\output\\logfiles\\";
	
	public static void main(String[] args) {
		
		System.out.println("FCFS");
		
		int[][] FCFS_C = IOScanner.getData(path+"FCFS_C.log");

		double talUtil = 0;
		int endTime = FCFS_C[FCFS_C.length-1][0];
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
		
		double curPow = 0;
		double talPow = 0;
		double maxCurPow = 0;
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
			if(curPow>maxCurPow) {
				maxCurPow = curPow;
			}
		}
	
		System.out.println("Energy Consumption: " + df.format(talPow) + " MaxPow: " + maxCurPow);
		System.out.println("EDP: " + df.format(talPow*(endTime-strTime)));
		
		/* ******************************************************************************************* */
		
		System.out.println("\nDVFS");
		
		int[][] DVFS_C = IOScanner.getData(path+"DVFS1_C.log");

		talUtil = 0;
		endTime = DVFS_C[DVFS_C.length-1][0];
		strTime = DVFS_C[0][0];
			
		for(int i=0; i<DVFS_C.length-1; i++) {
			if(DVFS_C[i][0]!=DVFS_C[i+1][0]) {
				talUtil += (DVFS_C[i+1][0]-DVFS_C[i][0]) * (48-DVFS_C[i][1]);
//				System.out.println((DVFS_C[i+1][0]-DVFS_C[i][0]) * (48-DVFS_C[i][1]));
			}
		}
	
		System.out.println("System Utilization: " + df.format(talUtil/endTime/48));
		
		int[][] DVFS_Submit = IOScanner.getData(path+"DVFS1_Submit.log");
		int[][] DVFS_Run = IOScanner.getData(path+"DVFS1_Run.log");
		
		avgWT = 0;
			
		for(int i=0; i<DVFS_Submit.length; i++) {
			avgWT += DVFS_Run[i][0] - DVFS_Submit[i][0];
		}
	
		System.out.println("Average Time: " + df.format(avgWT/DVFS_Submit.length));
		
		int[][] DVFS_Release = IOScanner.getData(path+"DVFS1_Release.log");
		
		curPow = 0;
		talPow = 0;
		double powrate = 1.0;
		
		for(int i=0; i<=endTime; i++) {

			if(curPow>=80) {
				powrate = 0.8*0.8;
			}
			else if(curPow>=60&&curPow<80) {
				powrate = 0.9*0.9;
			}
			else if(curPow<60) {
				powrate = 1.0*1.0;
			}
			
			talPow += powrate*curPow;
			
			for(int j=0; j<DVFS_Run.length; j++) {
				if(DVFS_Run[j][0]==i) {
					curPow += DVFS_Run[j][1];
				}
				if(DVFS_Release[j][0]==i) {
					curPow -= DVFS_Release[j][1];
				}
			}
		}
	
		System.out.println("Energy Consumption: " + df.format(talPow));
		System.out.println("EDP: " + df.format(talPow*(endTime-strTime)));
		
		/* ******************************************************************************************* */
		
		System.out.println("\nBLOCK");
		
		int[][] BLOCK_C = IOScanner.getData(path+"BLOCK_C.log");

		talUtil = 0;
		endTime = BLOCK_C[BLOCK_C.length-1][0];
		strTime = BLOCK_C[0][0];
			
		for(int i=0; i<BLOCK_C.length-1; i++) {
			if(BLOCK_C[i][0]!=BLOCK_C[i+1][0]) {
				talUtil += (BLOCK_C[i+1][0]-BLOCK_C[i][0]) * (48-BLOCK_C[i][1]);
//				System.out.println((BLOCK_C_Data[i+1][0]-BLOCK_C_Data[i][0]) * (48-BLOCK_C_Data[i][1]));
			}
		}
	
		System.out.println("System Utilization: " + df.format(talUtil/endTime/48));
		
		int[][] BLOCK_Submit = IOScanner.getData(path+"BLOCK_Submit.log");
		int[][] BLOCK_Run = IOScanner.getData(path+"BLOCK_Run.log");
		
		avgWT = 0;
			
		for(int i=0; i<BLOCK_Submit.length; i++) {
			avgWT += BLOCK_Run[i][0] - BLOCK_Submit[i][0];
		}
	
		System.out.println("Average Time: " + df.format(avgWT/BLOCK_Submit.length));
		
		int[][] BLOCK_Release = IOScanner.getData(path+"BLOCK_Release.log");
		
		curPow = 0;
		talPow = 0;
		for(int i=0; i<=endTime; i++) {
			talPow += curPow;
			for(int j=0; j<BLOCK_Run.length; j++) {
				if(BLOCK_Run[j][0]==i) {
					curPow += BLOCK_Run[j][1];
				}
				if(BLOCK_Release[j][0]==i) {
					curPow -= BLOCK_Release[j][1];
				}
			}
		}
	
		System.out.println("Energy Consumption: " + df.format(talPow));
		System.out.println("EDP: " + df.format(talPow*(endTime-strTime)));
		
		/* ******************************************************************************************* */
		
		
		System.out.println("\nWAIT");
		
		int[][] WAIT_C = IOScanner.getData(path+"WAIT_C.log");

		talUtil = 0;
		endTime = WAIT_C[WAIT_C.length-1][0];
		strTime = WAIT_C[0][0];
			
		for(int i=0; i<WAIT_C.length-1; i++) {
			if(WAIT_C[i][0]!=WAIT_C[i+1][0]) {
				talUtil += (WAIT_C[i+1][0]-WAIT_C[i][0]) * (48-WAIT_C[i][1]);
//				System.out.println((WAIT_C_Data[i+1][0]-WAIT_C_Data[i][0]) * (48-WAIT_C_Data[i][1]));
			}
		}
	
		System.out.println("System Utilization: " + df.format(talUtil/endTime/48));
		
		int[][] WAIT_Submit = IOScanner.getData(path+"WAIT_Submit.log");
		int[][] WAIT_Run = IOScanner.getData(path+"WAIT_Run.log");
		
		avgWT = 0;
			
		for(int i=0; i<WAIT_Submit.length; i++) {
			avgWT += WAIT_Run[i][0] - WAIT_Submit[i][0];
		}
	
		System.out.println("Average Time: " + df.format(avgWT/WAIT_Submit.length));
		
		int[][] WAIT_Release = IOScanner.getData(path+"WAIT_Release.log");
		
		curPow = 0;
		talPow = 0;
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
		}
	
		System.out.println("Energy Consumption: " + df.format(talPow));
		System.out.println("EDP: " + df.format(talPow*(endTime-strTime)));
		
	}
	
}
	