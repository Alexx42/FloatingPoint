#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>

# define PARSE_ERROR "Syntax error: impossible to parse the number."

# define FLOAT_ERROR "Float error: impossible to convert to float."


# define ARCH_BITS 0x20

# define POW32 0x100000000

# define MASK_EXPOSANT 0x7F000000
# define MASK_MANTICE 0x00FFFFFF

# define GET_SIGN(x) ((x & POW32) >> 32)

# define GET_EXPOSANT(x) ((x & MASK_EXPOSANT) >> 23)

# define GET_MANTICE(x) ((x &  MASK_MANTICE) >> 15)

typedef struct	s_float32 {
	uint32_t	natural;
	uint32_t	decimal;
	float		IEEE;
}		float32;

int		get_size_number(int x) {
	int		count;

	count = 0;
	while (x > 0) {
		count++;
		x /= 10;
	}
	return count;
}

void		print_binary(float x, size_t size) {
	long i = size;
	int count = 0;
	int _x;
	
	memcpy(&_x, &x, sizeof(_x));
	while (i >>= 1) {
		_x & i ? printf("1") : printf("0");
		count++;
		if (count % 4 == 0)
			printf(" ");
	}
	printf("\n");
	return ;
}

int		ret_error(const char *msg) {
	printf("%s\n", msg);
	return EXIT_FAILURE;
}

int		find_separation(char *nb) {
	int		i;
	int		count;

	if (nb == NULL)
		return 0;
	count = 0;
	i = -1;
	if (nb[i + 1] && nb[i + 1] == '.')
	       i++;
	while (++i < strlen(nb)) {
		if (isdigit(nb[i]))
			count++;
		else if (nb[i] == '.')
			break ;
	}
	return count;
}

void		print_infos(float32 *f) {
	printf("Natural: %d\n", f->natural);
	printf("Decimal: %d\n", f->decimal);

	printf("\n-----------Representation of the number----------------\n");
	printf("Decimal: %.2f\n", f->IEEE);
	printf("Binary: ");
	print_binary(f->IEEE, POW32);
}


float32		create_float32() {
	float32		f;

	f.natural = 0;
	f.decimal = 0;
	f.IEEE = 0;
	return f;
}

int		parse_float(char *nb, float32 *f) {
	
	int		i;
	int		j;
	int		index_nat;
	int		index_dec;
	char		flag;

	index_dec = 0;
	flag = 0x00;
	if (nb == NULL || strlen(nb) <= 0) {
		return EXIT_FAILURE;
	}
	i = -1;
	index_nat = find_separation(nb);
	index_dec = find_separation(strchr(nb, '.') + (nb[0] == '-' ? 1 : 0));
	while (++i < strlen(nb)) {
		if (!isdigit(nb[i]) && nb[i] != '.' && nb[i] != '-')
			return EXIT_FAILURE;
		if (nb[i] != '.' && nb[i] != '-') {
			if (!flag) {
				index_nat--;
				f->natural = f->natural + (nb[i] - '0') * pow(10, index_nat);
			}
			else {
				index_dec--;
				f->decimal = (f->decimal + ((nb[i] - '0') * pow(10, index_dec)));
			}
		}
		else if (nb[i] == '.') {
			flag = 0x01;
		}
	}
	return EXIT_SUCCESS;
}

void	step_mantice(float x, char *exposant) {
	int		i;
	int		_x;
	int		exp;

	i = 23;
	memcpy(&_x, &x, sizeof(_x));
	printf("1 * (");
	exp = (int)log2(x * (x < 0 ? -1 : 1));
	while (--i >= 0) {
		if (_x & (1 << i)) {
			printf("1 / 2^%d + ", 23 - i);
            printf("\t");
		}
	}
	printf(")");
	printf(" * 2^%d)\n", exp);
	return ;
}

void		parse_IEE_754(float x) {
	char		sign;
	char		exposant[8];
	char		mantice[23];
	int			_x;
	int			i;
	int			j;
	int			k;

	memcpy(&_x, &x, sizeof(x));
	sign = (_x & (POW32)) >> 32;
	printf("\n-------------IEEE-754------------\n");
	printf("Signe: %d\n", sign);
	i = 30;
	j = -1;
	k = -1;
	while (i > 0) {
		if (i > 23) {
			if (_x & (1 << i)) {
				exposant[++j] = '1';
			} else {
				exposant[++j] = '0';
			}
		} else {
			if (_x & (1 << i)) {
            	mantice[++k] = '1';
			} else {
				mantice[++k] = '0';
			}
		}
		i--;
	}
	exposant[j + 1] = '\0';
	mantice[k + 1] = '\0';
	printf("Exposant: %s\n", exposant);
	printf("Mantice: %s\n", mantice);
	step_mantice(x, exposant);
	return ;
}




int		main(int ac, char **av) {
	float32			f;

	f = create_float32();
	if (ac != 2) {
		printf("usage: ./float [32 bits digits number]\n");
		return 1;
	}
	if (parse_float(av[1], &f) == EXIT_FAILURE) {
		return ret_error(PARSE_ERROR);
	}
	f.IEEE = (f.natural + (f.decimal / pow(10, get_size_number(f.decimal)))) * (av[1][0] == '-' ? -1 : 1);
	print_infos(&f);
	parse_IEE_754(f.IEEE);
	return 0;
}


