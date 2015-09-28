import java.util.*;
import java.io.*;

public class IOScanner {
	
	public static int[][] getData(String fileName) {
		
		Vector<String> dataSet = new Vector<String>();
		String oneLine = "";
		int[][] data = new int[1][1];
		
		try {
			Scanner sc = new Scanner(new File(fileName));
			while(sc.hasNextLine()) {
				oneLine = sc.nextLine();
				dataSet.add(oneLine);
			}
			sc.close();
			int row = dataSet.size()-1;
			data = new int[row][2];
			for(int i=1; i<row+1; i++) {
				sc = new Scanner(dataSet.elementAt(i)).useDelimiter(" ");
				data[i-1][1] = sc.nextInt();
				for(int j=0; j<2; j++) {
					sc.next();
				}
				data[i-1][0] = sc.nextInt();
				sc.close();
			}
			sc.close();
		} catch(Exception e) {
			e.printStackTrace();
			System.exit(0);
		}
		return data;
	}
	
}
