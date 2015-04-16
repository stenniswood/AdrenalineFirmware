#include <stdio.h>



int main()
{
	char sentence[] = "Kp_position_alpha=123.654\n";
	float Kp_position_alpha;
	
	sscanf( sentence, "Kp_position_alpha=%6.3f\n", &Kp_position_alpha );
	
	printf("Kp=%6.3f\n", Kp_position_alpha );
	
}