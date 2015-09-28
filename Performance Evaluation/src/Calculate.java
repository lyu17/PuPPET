import java.util.*;
import java.io.*;

public class Calculate {
	
	public static int getCT(String fileName) {
		
		String path = fileName + "_C.log";
		int[][] util = IOScanner.getData(path);
		
		int endTime = util[util.length-1][0];
		int strTime = util[0][0];
		
		return (endTime-strTime);
		
	}
	
	public static double getUtil(String fileName) {
		
		String path = fileName + "_C.log";
		int[][] util = IOScanner.getData(path);

		double talUtil = 0;
		int endTime = util[util.length-1][0];
		int strTime = util[0][0];
			
		for(int i=0; i<util.length-1; i++) {
			if(util[i][0]!=util[i+1][0]) {
				talUtil += (util[i+1][0]-util[i][0]) * (48-util[i][1]);
				
//				System.out.print((util[i+1][0]-util[i][0]) * (48-util[i][1]));
				
			}
		}
		double sysUtil = talUtil/(endTime-strTime)/48;
		return sysUtil;
		
	}
	
	public static double getAWT(String fileName) {
		
		String path1 = fileName + "_Submit.log";
		String path2 = fileName + "_Run.log";
		
		int[][] submit = IOScanner.getData(path1);
		int[][] run = IOScanner.getData(path2);

		double waittime = 0;
		for(int i=0; i<submit.length; i++) {
			waittime += run[i][0] - submit[i][0];
		}	
		
		double avgwt = waittime/submit.length;
		return avgwt;
		
	}
	
	public static double getEnergy(String fileName) {
		
		String path1 = fileName + "_C.log";
//		String path2 = fileName + "_Submit.log";
		String path3 = fileName + "_Run.log";
		String path4 = fileName + "_Release.log";
		
		int[][] util = IOScanner.getData(path1);
//		int[][] submit = IOScanner.getData(path2);
		int[][] run = IOScanner.getData(path3);
		int[][] release = IOScanner.getData(path4);
		
		double curPow = 0;
		double talPow = 0;
		double edp = 0;
		
		int endTime = util[util.length-1][0];
		int strTime = util[0][0];
		
		Vector<Integer> curUtils = new Vector<Integer>();
		curUtils.addElement(0);
		for(int i=0; i<util.length-1; i++) {
			if(util[i][0]!=util[i+1][0]) {
				for(int j=0; j<(util[i+1][0]-util[i][0]); j++) {
					curUtils.addElement(util[i][1]);
				}
			}
		}
		
		for(int i=0; i<=endTime; i++) {
//			talPow += curPow + 1*curUtils.elementAt(i);
//			talPow += curPow + 22*curUtils.elementAt(i);
//			talPow += curPow + 44*curUtils.elementAt(i);
			talPow += curPow + 67*curUtils.elementAt(i);
//			talPow += curPow + 89*curUtils.elementAt(i);
//			talPow += curPow + 112*curUtils.elementAt(i);
			for(int j=0; j<run.length; j++) {
				if(run[j][0]==i) {
					curPow += run[j][1];
				}
				if(release[j][0]==i) {
					curPow -= release[j][1];
				}
			}
		}
		
		return talPow;
		
	}
	
	public static double getDVFSEnergy(int[] threshold, String fileName) {
		
		String path1 = fileName + "_C.log";
//		String path2 = fileName + "_Submit.log";
		String path3 = fileName + "_Run.log";
		String path4 = fileName + "_Release.log";
		
		int[][] util = IOScanner.getData(path1);
//		int[][] submit = IOScanner.getData(path2);
		int[][] run = IOScanner.getData(path3);
		int[][] release = IOScanner.getData(path4);
		
		double curPow = 0;
		double talPow = 0;
		double powrate = 1.0;
		double edp = 0;
		
		int endTime = util[util.length-1][0];
		int strTime = util[0][0];
		
		Vector<Integer> curUtils = new Vector<Integer>();
		curUtils.addElement(0);
		for(int i=0; i<util.length-1; i++) {
			if(util[i][0]!=util[i+1][0]) {
				for(int j=0; j<(util[i+1][0]-util[i][0]); j++) {
					curUtils.addElement(util[i][1]);
				}
			}
		}
		
		for(int i=0; i<=endTime; i++) {

			if(curPow>=threshold[1]) {
				powrate = 0.8;
			}
			else if(curPow>=threshold[0]&&curPow<threshold[1]) {
				powrate = 0.9;
			}
			else if(curPow<threshold[0]) {
				powrate = 1.0;
			}
//			talPow += powrate*curPow + 1*curUtils.elementAt(i);
//			talPow += powrate*curPow + 22*curUtils.elementAt(i);
//			talPow += powrate*curPow + 44*curUtils.elementAt(i);
			talPow += powrate*curPow + 67*curUtils.elementAt(i);		
//			talPow += powrate*curPow + 89*curUtils.elementAt(i);
//			talPow += powrate*curPow + 112*curUtils.elementAt(i);
			
			for(int j=0; j<run.length; j++) {
				if(run[j][0]==i) {
					curPow += run[j][1];
				}
				if(release[j][0]==i) {
					curPow -= release[j][1];
				}
			}
		}
		
		return talPow;
		
	}
	
}
