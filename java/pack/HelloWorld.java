package pack;

class HelloWorld implements FakeInterface, FakeInterfaceTwo
{
	/*
     public final byte sbyte = 0x10;
     public final char character = 'a';
	public final int integer = -123456;
	public final int integer2 = 987654;
	public final float floating_point = 123.456f;
	public final long long_integer = -123456789;
	public final double double_precision = 1234.5678;
     
     public final static String string = "stringvalue";
     public final short short_integer = 1;
     public final boolean bool = true;  
     
     */
	public final int intarray[] = {0, 1, 2};
     public static final String stringarray[] = {"a", "b", "c"};
     public static final FakeInterface interfacearray[] = {null, null};

	static int i;
     public HelloWorld()
     {
          System.out.println("Constructor");
     }
    	public static void main(String[] args)
	{
          //System.out.println("Hello world!");
          //System.out.println(string);
		String array[] = new String[10];
		i = 10;
		int j = 100;
		float f = 0.5f;
		
		float result = (float) (i+j) * f;
		System.out.println(result);

          String shit = stringarray[3];
		
	}
     
     public static void samemethod(String[] args)
     {
		System.out.println(i);
     }
     public static void samemethod(String[] args, int i)
     {
     }
     
     @Override
     public void InterfaceMethod1()
     {
     }
     @Override
     public void InterfaceMethod2(int i)
     {
     }
     @Override
     public void InterfaceMethod1_Two()
     {
     }
     @Override
     public void InterfaceMethod2_Two(int i)
     {
     }
     
     public int TestMethod(int i, double d, String s, Object o)
     {
          return 0;
     }
}
