typedef char (&tempHolder)[10][100];
struct element_{
   char dest;
   char dist[5]; 
};
struct distance_vector{
	char sender;
	char number_of_dests[3];
	struct element_ content[7];
};
struct local_machine{

	char name;
	int port_num;
	char link_info[7][5][100];
};