#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>







int
main(int ac, char *av[])
{	
	unsigned int data = 0;
	user_reg_io_t regio;
	unsigned int re;
	int fd;
	int rd_cmd;	
	;

	
	
		
		
			

			if (read)
			{
				_init_gpio_in(fDev_io_gpio, pin);

				data = _get_gpio(fDev_io_gpio, pin);
			}
			else // write
			{
				if (data)
					data = GPIO_VALUE_HIGH;
				else
					data = GPIO_VALUE_LOW;

				_init_gpio_out(fDev_io_gpio, pin, data);

				_set_gpio(fDev_io_gpio, pin, data);
			}
					
		
		
	

	return 0;
}

