import java.util.*;

public class IMGenerator {
    	
	static double seftRandom(double[] r){
		double base,u,v,p,temp1,temp2,temp3;
		base = 256.0;
		u = 17.0;
		v = 139.0;
		temp1 = u*(r[0])+v;
		temp2 = (int)(temp1/base);
		temp3 = temp1 - temp2*base;
		r[0] = temp3;
		p = r[0]/base;
		return p;
	}

	static int randZT(double u,double t,double[]r,double n){
		int i;
		double total = 0.0;
		double result;
		for(i = 0;i<n;i++){
			total += seftRandom(r);
		}
		result = u+t*((total-n/2.0)/Math.sqrt(n/12));
		int num = (int)result;
		if(num>u+t||num<u-t||num<=0) {
			num = (int)u;
		}
		return num;
	}
    	
	private static int getPoissonRandom(double mean) {
	    Random r = new Random();
	    double L = Math.exp(-mean);
	    int k = 0;
	    double p = 1.0;
	    do {
	        p = p * r.nextDouble();
	        k++;
	    } while (p > L);
	    return k - 1;
	}
	
	public static int getRandomNum(int[] arr, int[] probability){
        if(arr.length != probability.length) return Integer.MIN_VALUE;
        Random ran = new Random();
        int ran_num = ran.nextInt(100);
        int temp = 0;
        for (int i = 0; i < arr.length; i++) {
            temp += probability[i];
            if(ran_num < temp)
                return arr[i];
        }
        return Integer.MIN_VALUE;
    }
    
    public static void main(String[] args) {
    	
    	int st = 0;
		double[] r = {5.0};
		int n = 10;
    	
    	for(int i=0; i<500; i++) {
    		int rack = getRandomNum(new int[]{1,2,4,8,12,16,24,32,48}, new int[]{73,12,10,4,0,1,0,0,0});
    		int rt = randZT(40,50,r,n);
    		int pow = randZT(23,60,r,n);
    		if(pow<5) {
    			pow = 5;
    		}
    		System.out.println("1`("+rack+","+rt+","+pow+",1.0)@+"+st+"++");
    		int icr = getPoissonRandom(4.2);
    		if(icr==0) {
    			st++;
    		}
    		else {
    			st += icr;
    		}
    	}
    }
    	
}
    	