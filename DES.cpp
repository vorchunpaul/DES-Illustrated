#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <map>

#define BINTYPE		short
#define BINEVECTOR	std::vector<short>

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */

#define BLOCK_SIZE 64

#define PERMUTATION_START_MODE 1
#define PERMUTATION_END_MODE 0

/**
  TDD Coding Style ;-)
 */

std::string BINVECT_out(BINEVECTOR vect, unsigned mode);

BINEVECTOR stringToBytes(std::string str)
{
	BINEVECTOR byts;
	for (auto c : str)
	{
		if (c == '0')
			byts.push_back(0);

		if (c == '1')
			byts.push_back(1);
	}
	return byts;
}

bool test_strToByts()
{
	std::string IN = "0000000000000010000000000000000000000000000000000000000000000001";
	BINEVECTOR OUT = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};

	BINEVECTOR test = stringToBytes(IN);

	return test == OUT;
}

std::pair<BINEVECTOR, BINEVECTOR> Key64toPair28keys(BINEVECTOR key64)
{
	std::vector<short> tableKey =
	{
		57,	49,	41,	33,	25,	17,	 9,	 1,	58,	50,	42,	34,	26,	18,
		10,	 2,	59,	51,	43,	35,	27,	19,	11,	 3,	60,	52,	44,	36,
		63,	55,	47,	39,	31,	23,	15,	 7,	62,	54,	46,	38,	30,	22,
		14,	 6,	61,	53,	45,	37,	29,	21,	13,	 5,	28,	20,	12,	 4
	};

	BINEVECTOR key, keyC, keyD;

	for (auto i : tableKey)
	{
		key.push_back(key64[i-1]);
	}

	for (int i = 0; i < 28; ++i)
		keyC.push_back(key[i]);

	for (unsigned i = 28; i < key.size(); ++i)
		keyD.push_back(key[i]);

	return { keyC, keyD };
}

bool test_make_pair_keys()
{
	std::string IN = "10000010 00000000 00101000 00000000 00000000 00000000 00000000 00000000";
	std::string OUT1 = "0000000100000000000001000000";
	std::string OUT2 = "0000000100000000000001000000";

	std::pair<BINEVECTOR, BINEVECTOR> pair = Key64toPair28keys(stringToBytes(IN));
	return (pair.first == stringToBytes(OUT1)) && (pair.second == stringToBytes(OUT2));
}

BINEVECTOR permutation(BINEVECTOR input, bool mode)
{
	static std::vector<short> IPTABLE =
	{
		58, 50, 42, 34, 26, 18, 10,  2,
		60, 52, 44, 36, 28, 20, 12,  4,
		62, 54, 46, 38, 30, 22, 14,  6,
		64, 56, 48, 40, 32, 24, 16,  8,
		57, 49, 41, 33, 25, 17,  9,  1,
		59, 51, 43, 35, 27, 19, 11,  3,
		61, 53, 45, 37, 29, 21, 13,  5,
		63, 55, 47, 39, 31, 23, 15,  7
	};

	static std::vector<short> table2;
	static bool initt2 = false;
	if (!initt2)
	{
		table2.resize(IPTABLE.size(), 0);
		for (unsigned i = 0; i < IPTABLE.size(); i++)
		{
			table2[IPTABLE[i] - 1] = i + 1;
		}
		initt2 = true;
	}

	BINEVECTOR perm;
	
	if (mode)
	{
		for (auto i : IPTABLE)
			perm.push_back(input[i - 1]);

		return perm;
	}
	else
	{
		for (auto i : table2)
			perm.push_back(input[i - 1]);

		return perm;
	}

}

bool test_permutation_start()
{
	std::string IN =  "0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111";
	std::string OUT = "1100 1100 0000 0000 1100 1100 1111 1111 1111 0000 1010 1010 1111 0000 1010 1010";

		return (stringToBytes(OUT) == permutation(stringToBytes(IN), PERMUTATION_START_MODE));
}

bool test_permutation_end()
{
	std::string IN  = "00001010 01001100 11011001 10010101 01000011 01000010 00110010 00110100 ";
	std::string OUT = "10000101 11101000 00010011 01010100 00001111 00001010 10110100 00000101 ";

	return (stringToBytes(OUT) == permutation(stringToBytes(IN), PERMUTATION_END_MODE));

}

BINEVECTOR LeftShift(BINEVECTOR input)
{
	BINEVECTOR shift;
	
	for (unsigned i = 1; i < input.size(); ++i)
	{
		shift.push_back(input[i]);
	}

	shift.push_back(input[0]);

	return shift;
}

bool test_leftshift()
{
	std::string IN  = "0000 0000 0000 0000 0000 0000 1111";
	std::string OUT = "0000 0000 0000 0000 0000 0001 1110";

	return stringToBytes(OUT) == LeftShift(stringToBytes(IN));
}

BINEVECTOR DobleLeftShift(BINEVECTOR input)
{
	return LeftShift(LeftShift(input));
}

bool test_2shift()
{
	std::string IN =  "0000 0000 0000 0000 0000 0000 1111";
	std::string OUT = "0000 0000 0000 0000 0000 0011 1100";

	return stringToBytes(OUT) == DobleLeftShift(stringToBytes(IN));
}

BINEVECTOR PairKeyToOne(BINEVECTOR key1, BINEVECTOR key2)
{
	BINEVECTOR key;

	for (auto i : key1)
		key.push_back(i);
	for (auto i : key2)
		key.push_back(i);

	return key;
}

bool test_pairtoone()
{
	std::string IN1 = "0000 0000 0000 0000 0000 0000 0000";
	std::string IN2 = "1111 1111 1111 1111 1111 1111 1111";
	std::string OUT = "0000 0000 0000 0000 0000 0000 0000 1111 1111 1111 1111 1111 1111 1111";

	return stringToBytes(OUT) == PairKeyToOne(stringToBytes(IN1), stringToBytes(IN2));
}

BINEVECTOR compresKey(BINEVECTOR Key)
{
	std::vector<short> table =
	{
		14, 17, 11, 24,  1,  5,
		 3, 28, 15,  6, 21, 10,
		23, 19, 12,  4, 26,  8,
		16,  7, 27, 20, 13,  2,
		41, 52, 31, 37, 47, 55,
		30, 40, 51, 45, 33, 48,
		44, 49, 39, 56, 34, 53,
		46, 42, 50, 36, 29, 32
	};

	BINEVECTOR compress;

	for (auto i : table)
		compress.push_back(Key[i - 1]);

	return compress;
}

bool test_compresKey()
{
	std::string IN = "1110000 1100110 0101010 1011111 1010101 0110011 0011110 0011110";
	std::string OUT = "000110 110000 001011 101111 111111 000111 000001 110010";

	return stringToBytes(OUT) == compresKey(stringToBytes(IN));
}

std::vector<BINEVECTOR> genation16raundKeys(BINEVECTOR keyС, BINEVECTOR keyD)
{
	std::map<int, int> shiftTable =
	{
		{ 1, 1},	// this
		{ 2, 1},	// this
		{ 3, 2},
		{ 4, 2},
		{ 5, 2},
		{ 6, 2},
		{ 7, 2},
		{ 8, 2},
		{ 9, 1},	// this
		{10, 2},
		{11, 2},
		{12, 2},
		{13, 2},
		{14, 2},
		{15, 2},
		{16, 1}		// this
	};

	std::vector<BINEVECTOR> keys;
	BINEVECTOR _keyС = keyС;
	BINEVECTOR _keyD = keyD;
	BINEVECTOR key;
	BINEVECTOR compresskey;

	unsigned key_i = 0;
	
	for (auto i : shiftTable)
	{
		
		
		if (i.second == 1)
		{
			_keyС = LeftShift(_keyС);
			_keyD = LeftShift(_keyD);
		}

		if (i.second == 2)
		{
			_keyС = DobleLeftShift(_keyС);
			_keyD = DobleLeftShift(_keyD);
		}

		key = PairKeyToOne(_keyС, _keyD);

		std::string nullstr; if (key_i < 9) nullstr = '0'; else nullstr = "";

		std::cout << "K" << nullstr << key_i + 1 << " =\t" << BINVECT_out(_keyС, 28) << " " << BINVECT_out(_keyD, 28) << '\n';
		key_i++;
		compresskey = compresKey(key);

		keys.push_back(compresskey);
	}

	return keys;
}

bool test_genation16raundKeys()
{
	std::string KEYC = "1111000011001100101010101111";
	std::string KEYD = "0101010101100110011110001111";

	std::vector<std::string> OUT =
	{
		"000110 110000 001011 101111 111111 000111 000001 110010",
		"011110 011010 111011 011001 110110 111100 100111 100101",
		"010101 011111 110010 001010 010000 101100 111110 011001",
		"011100 101010 110111 010110 110110 110011 010100 011101",
		"011111 001110 110000 000111 111010 110101 001110 101000",
		"011000 111010 010100 111110 010100 000111 101100 101111",
		"111011 001000 010010 110111 111101 100001 100010 111100",
		"111101 111000 101000 111010 110000 010011 101111 111011",
		"111000 001101 101111 101011 111011 011110 011110 000001",
		"101100 011111 001101 000111 101110 100100 011001 001111",
		"001000 010101 111111 010011 110111 101101 001110 000110",
		"011101 010111 000111 110101 100101 000110 011111 101001",
		"100101 111100 010111 010001 111110 101011 101001 000001",
		"010111 110100 001110 110111 111100 101110 011100 111010",
		"101111 111001 000110 001101 001111 010011 111100 001010",
		"110010 110011 110110 001011 000011 100001 011111 110101"
	};

	std::vector<BINEVECTOR> out_bin;
	for (auto i : OUT)
		out_bin.push_back(stringToBytes(i));

	
	return out_bin == genation16raundKeys(stringToBytes(KEYC), stringToBytes(KEYD));
}

BINEVECTOR fundtionE(BINEVECTOR M)
{
	std::vector<short> table =
	{
		 32,     1,    2,     3,     4,    5,
		  4,     5,    6,     7,     8,    9,
		  8,     9,   10,    11,    12,   13,
		 12,    13,   14,    15,    16,   17,
		 16,    17,   18,    19,    20,   21,
		 20,    21,   22,    23,    24,   25,
		 24,    25,   26,    27,    28,   29,
		 28,    29,   30,    31,    32,    1
	};

	BINEVECTOR extertM;
	for (auto i : table)
	{
		extertM.push_back(M[i - 1]);
	};

	return extertM;
}

bool test_fundtionE()
{
	std::string IN1 = "1111 0000 1010 1010 1111 0000 1010 1010";
	std::string OUT = "011110 100001 010101 010101 011110 100001 010101 010101";

	return stringToBytes(OUT) == fundtionE(stringToBytes(IN1));
}

BINTYPE binXor(BINTYPE a, BINTYPE b)
{
	return a ^ b;
}

bool test_binXor()
{
	return		(binXor(0, 0) == 0)
			&&	(binXor(0, 1) == 1)
			&&	(binXor(1, 0) == 1)
			&&	(binXor(0, 0) == 0);
}

BINEVECTOR xor_binvector(BINEVECTOR first, BINEVECTOR second)
{
	BINEVECTOR output;

	for (unsigned i = 0; i < first.size(); i++)
	{
		output.push_back(binXor(first[i], second[i]));
	}

	return output;
}

bool test_xor_binvector()
{
	std::string IN1 = "000110 110000 001011 101111 111111 000111 000001 110010";
	std::string IN2 = "011110 100001 010101 010101 011110 100001 010101 010101";
	std::string OUT = "011000 010001 011110 111010 100001 100110 010100 100111";

	return stringToBytes(OUT) == xor_binvector(stringToBytes(IN1), stringToBytes(IN2));
}

BINTYPE SblockItem(unsigned index, int i, int j)
{
	std::vector<std::vector<std::vector<BINTYPE>>> SBlockTable =
	{
		{
			{14,	 4,	13,	 1,	 2,	15,	11,	 8,	 3,	10,	 6,	12,	 5,	 9,	 0,	 7},
			{ 0,	15,	 7,	 4,	14,  2,	13,	 1,	10,	 6,	12,	11,	 9,	 5,	 3,	 8},
			{ 4,	1,	14,	 8,	13,	 6,	 2,	11,	15,	12,	 9,	 7,	 3,	10,	 5,	 0},
			{15,	12,	 8,	 2,	 4,	 9,	 1,	 7,	 5,	11,	 3,	14,	10,	 0,	 6,	 13}
		},
		{
			{15,	1,	8,	14,	6,	11,	3,	4,	9,	7,	2,	13,	12,	0,	5,	10},
			{3,	13,	4,	7,	15,	2,	8,	14,	12,	0,	1,	10,	6,	9,	11,	5},
			{0,	14,	7,	11,	10,	4,	13,	1,	5,	8,	12,	6,	9,	3,	2,	15},
			{13,	8,	10,	1,	3,	15,	4,	2,	11,	6,	7,	12,	0,	5,	14,	9}

		},
		{
			{10,0,	9,	14,	6,	3,	15,	5,	1,	13,	12,	7,	11,	4,	2,	8},
			{13,7,	0,	9,	3,	4,	6,	10,	2,	8,	5,	14,	12,	11,	15,	1},
			{13,6,	4,	9,	8,	15,	3,	0,	11,	1,	2,	12,	5,	10,	14,	7},
			{1,	10,	13,	0,	6,	9,	8,	7,	4,	15,	14,	3,	11,	5,	2,	12}

		},
		{
			{7,	13,	14,	3,	0,	6,	9,	10,	1,	2,	8,	5,	11,	12,	4,	15},
			{13,	8,	11,	5,	6,	15,	0,	3,	4,	7,	2,	12,	1,	10,	14,	9},
			{10,	6,	9,	0,	12,	11,	7,	13,	15,	1,	3,	14,	5,	2,	8,	4},
			{3,	15,	0,	6,	10,	1,	13,	8,	9,	4,	5,	11,	12,	7,	2,	14}
		},
		{
			{2,	12,	4,	1,	7,	10,	11,	6,	8,	5,	3,	15,	13,	0,	14,	9},
			{14,	11,	2,	12,	4,	7,	13,	1,	5,	0,	15,	10,	3,	9,	8,	6},
			{4,	2,	1,	11,	10,	13,	7,	8,	15,	9,	12,	5,	6,	3,	0,	14},
			{11,	8,	12,	7,	1,	14,	2,	13,	6,	15,	0,	9,	10,	4,	5,	3}

		},
		{
			{12,	1,	10,	15,	9,	2,	6,	8,	0,	13,	3,	4,	14,	7,	5,	11},
			{10,	15,	4,	2,	7,	12,	9,	5,	6,	1,	13,	14,	0,	11,	3,	8},
			{9,	14,	15,	5,	2,	8,	12,	3,	7,	0,	4,	10,	1,	13,	11,	6},
			{4,	3,	2,	12,	9,	5,	15,	10,	11,	14,	1,	7,	6,	0,	8,	13}
		},
		{
			{4,	11,	2,	14,	15,	0,	8,	13,	3,	12,	9,	7,	5,	10,	6,	1},
			{13,	0,	11,	7,	4,	9,	1,	10,	14,	3,	5,	12,	2,	15,	8,	6},
			{1,	4,	11,	13,	12,	3,	7,	14,	10,	15,	6,	8,	0,	5,	9,	2},
			{6,	11,	13,	8,	1,	4,	10,	7,	9,	5,	0,	15,	14,	2,	3,	12}
		},
		{
			{13,	2,	8,	4,	6,	15,	11,	1,	10,	9,	3,	14,	5,	0,	12,	7},
			{1,	15,	13,	8,	10,	3,	7,	4,	12,	5,	6,	11,	0,	14,	9,	2},
			{7,	11,	4,	1,	9,	12,	14,	2,	0,	6,	10,	13,	15,	3,	5,	8},
			{2,	1,	14,	7,	4,	10,	8,	13,	15,	12,	9,	0,	3,	5,	6,	11}
		}
	};

	return SBlockTable[index-1][i-1][j-1];
}

bool test_SblockItem()
{
	 return (SblockItem(1, 1, 1) == 14) && (SblockItem(8, 4, 16) == 11);
}

BINEVECTOR DecToBin4(unsigned n)
{
	BINEVECTOR nbin;

	while (n > 0)
	{
		nbin.push_back(n % 2);
		n /= 2;
	}

	while (nbin.size() < 4)
	{
		nbin.push_back(0);
	}

	std::reverse(nbin.begin(), nbin.end());

	return nbin;
}

bool test_DecToBin4()
{
	std::string OUT1 = "1010"; // 10
	std::string OUT2 = "1001"; // 9
	std::string OUT3 = "0100"; // 4
	std::string OUT4 = "0011"; // 3
	return (DecToBin4(10) == stringToBytes(OUT1))
		&& (DecToBin4(9) == stringToBytes(OUT2))
		&& (DecToBin4(4) == stringToBytes(OUT3))
		&& (DecToBin4(3) == stringToBytes(OUT4));
}

unsigned Bin4toDec(BINEVECTOR input)
{
	unsigned n = 0;
	unsigned m = 8;
	for (unsigned i = 0; i < input.size(); ++i)
	{
		n = n + (input[i] * m);
		m = m / 2;
	}

	return n;
}

bool test_Bin4toDec()
{
	std::string IN1 = "1010"; // 10
	std::string IN2 = "1001"; // 9
	std::string IN3 = "0100"; // 4
	std::string IN4 = "0011"; // 3

	return
		(Bin4toDec(stringToBytes(IN1)) == 10) &&
		(Bin4toDec(stringToBytes(IN2)) == 9) &&
		(Bin4toDec(stringToBytes(IN3)) == 4) &&
		(Bin4toDec(stringToBytes(IN4)) == 3);
}

BINEVECTOR Bit6toSblockto4bit(BINEVECTOR bit6, unsigned tableNum)
{
	BINEVECTOR bit4;
	BINEVECTOR rowBit, colBit;
	rowBit.push_back(0);
	rowBit.push_back(0);
	rowBit.push_back(bit6[0]);
	rowBit.push_back(bit6[5]);

	colBit.push_back(bit6[1]);
	colBit.push_back(bit6[2]);
	colBit.push_back(bit6[3]);
	colBit.push_back(bit6[4]);

	unsigned row = Bin4toDec(rowBit);
	unsigned col = Bin4toDec(colBit);
	auto dec = SblockItem(tableNum, row + 1, col + 1);
	bit4 = DecToBin4(dec);

	std::cout << "\t|______" << "TABLE___" << tableNum << "_________"<< "\n";
	std::cout << "\t| " << "\n";
	std::cout << "\t|  " << "bit6 = " << bit6[0] << " " << BINVECT_out(colBit, 5) << " " << bit6[5] << "\n";
	std::cout << "\t|  " << "table num = " << tableNum << "\n";
	std::cout << "\t|  " << "row = " << BINVECT_out(rowBit, 5) << " --> " << row + 1<< "\n";
	std::cout << "\t|  " << "col = " << BINVECT_out(colBit, 5) << " --> " << col + 1<< "\n";
	std::cout << "\t|  " << "S(table,row,col) --> " << "S(" << tableNum << ", " << row << ", " << col << ") " << "--> " << dec <<  " --> " << BINVECT_out(bit4, 5) << "\n";
	std::cout << "\t|  " << "Bit4 = " << BINVECT_out(bit4,5) << "\n";
	std::cout << "\t|  \n";
	return bit4;
}

bool test_Bit6toSblockto4bit()
{
	std::string IN1 = "1 1111 1"; // 4 row, 16 col
	std::string IN2 = "1 0000 0"; // 3 row, 1 col
	std::string IN3 = "0 1010 0"; // 1 row, 10 col

	BINEVECTOR OUT1 = Bit6toSblockto4bit(stringToBytes(IN1), 1); // 13
	BINEVECTOR OUT2 = Bit6toSblockto4bit(stringToBytes(IN2), 4); // 10
	BINEVECTOR OUT3 = Bit6toSblockto4bit(stringToBytes(IN3), 5); // 3

	return
		DecToBin4(13) == OUT1 &&
		DecToBin4(10) == OUT2 &&
		DecToBin4(3) == OUT3;
}

std::pair<BINEVECTOR, BINEVECTOR> VbinTo2Vbin(BINEVECTOR message)
{
	BINEVECTOR L;
	BINEVECTOR R;

	for (unsigned i = 0; i < message.size() / 2; ++i)
	{
		L.push_back(message[i]);
	}

	for (unsigned i = message.size() / 2; i < message.size(); ++i)
	{
		R.push_back(message[i]);
	}

	return { L,R };
}

bool test_VbinTo2Vbin()
{
	std::string IN = "0000 1111";
	std::string OUT1 = "0000";
	std::string OUT2 = "1111";

	return VbinTo2Vbin(stringToBytes(IN)).first == stringToBytes(OUT1) && VbinTo2Vbin(stringToBytes(IN)).second == stringToBytes(OUT2);
}

BINEVECTOR V2binToOne(BINEVECTOR L, BINEVECTOR R)
{
	BINEVECTOR one;
	for (auto i : L)
		one.push_back(i);

	for (auto i : R)
		one.push_back(i);

	return one;
}

bool test_V2binToOne()
{

	std::string IN1 = "0000";
	std::string IN2 = "1111";
	std::string OUT = "0000 1111";

	return V2binToOne(stringToBytes(IN1), stringToBytes(IN2)) == stringToBytes(OUT);
}

std::vector<BINEVECTOR> Vector42to8x6(BINEVECTOR Vect42)
{
	std::vector<BINEVECTOR> Bin6x8;

	for (int ii = 0; ii < 48; ii += 6)
	{
		BINEVECTOR bin6;
		bin6.clear();
		for (int i = ii; i < ii+6; ++i)
		{
			bin6.push_back(Vect42[i]);
		}
		Bin6x8.push_back(bin6);
	}

	return Bin6x8;
}

bool test_Vector42to8x6()
{
	std::string IN = "011000 010001 011110 111010 100001 100110 010100 100111";
	std::vector <std::string> OUT =
	{
		"011000",
		"010001",
		"011110",
		"111010",
		"100001",
		"100110",
		"010100",
		"100111"
	};

	std::vector<BINEVECTOR> OUTbin;
	for (auto i : OUT)
		OUTbin.push_back(stringToBytes(i));

	auto vect = Vector42to8x6(stringToBytes(IN));

	bool test = true;
	for (unsigned i = 0; i < OUTbin.size(); ++i)
	{
		test = OUTbin[i] == vect[i];
		if (!test) return test;
	}
	return test;
}

BINEVECTOR fundtionP(BINEVECTOR input)
{
	static std::vector<BINTYPE> PTable =
	{
		16,   7 , 20,  21,
		29,  12,  28,  17,
		 1,  15,  23,  26,
		 5,  18,  31,  10,
		 2,   8,  24,  14,
		32,  27,   3,   9,
		19,  13,  30,   6,
		22,  11,   4,  25
	};

	BINEVECTOR output;
	for (auto i : PTable)
	{
		output.push_back(input[i - 1]);
	}

	return output;
}

bool test_fundtionP()
{
	std::string IN = "0101 1100 1000 0010 1011 0101 1001 0111";
	std::string OUT = "0010 0011 0100 1010 1010 1001 1011 1011 ";

	return fundtionP(stringToBytes(IN)) == stringToBytes(OUT);
}

BINEVECTOR F_RAUND(BINEVECTOR R, BINEVECTOR RaundKey)
{
	BINEVECTOR raund;

	std::cout << "\n\t________F_RAUND_________" << "\n";
	auto ER = fundtionE(R);
	auto f = xor_binvector(ER, RaundKey);

	std::cout << "\t|  Extern text right block (E(R)): \n\t|    " << BINVECT_out(ER, 6) << "\n";
	std::cout << "\t|" << "\n";
	std::cout << "\t|  Raund key (K): \n\t|    " << BINVECT_out(RaundKey, 6) << "\n";
	std::cout << "\t|" << "\n";
	std::cout << "\t|  XOR (K+E(R)): \n\t|    " << BINVECT_out(f, 6) << "\n";
	std::cout << "\t|" << "\n";
	std::cout << "\t|____S__Table__raund_____"<< "\n";
	std::cout << "\t|" << "\n";
	auto f8 = Vector42to8x6(f);

	for (unsigned i = 0; i < f8.size(); ++i)
	{
		auto sblock = Bit6toSblockto4bit(f8[i], i+1);
		for (auto j : sblock)
			raund.push_back(j);
	}
	std::cout << "\t|________________________" << "\n";
	std::cout << "\t|" << "\n";
	std::cout << "\t|  S Block XOR (S(K+E(R)): \n\t|    " << BINVECT_out(raund, 4) << "\n";
	std::cout << "\t|" << "\n";
	raund = fundtionP(raund);

	std::cout << "\t|  P box of S Block XOR (P(S(K+E(R))): \n\t|    " << BINVECT_out(raund, 8) << "\n";
	std::cout << "\t|" << "\n";
	std::cout << "\t|_____END__F_RAUND_______" << "\n\n";
	return raund;
}

bool test_F_RAUND()
{
	std::string R =	  "1111 0000 1010 1010 1111 0000 1010 1010";
	std::string Key = "000110 110000 001011 101111 111111 000111 000001 110010";
	std::string OUT = " 0010 0011 0100 1010 1010 1001 1011 1011";

	auto raund = F_RAUND(stringToBytes(R),stringToBytes(Key));

	return raund == stringToBytes(OUT);
}

BINEVECTOR DES64(BINEVECTOR message64, BINEVECTOR key64)
{
	auto message = permutation(message64, PERMUTATION_START_MODE);

	auto pairCD = Key64toPair28keys(key64);

	std::cout << "KEY C and D (PC-1):\n\t"
		<< BINVECT_out(PairKeyToOne(pairCD.first, pairCD.second), 28) << "\n\n";

	auto keys = genation16raundKeys(pairCD.first, pairCD.second);

	std::cout << "\n\n";
	std::cout << "Complite compress raund keys (PC-2):\n";
	for (unsigned i = 0; i < keys.size(); ++i)
	{
		std::string Kn = "K"; if (i <= 8) Kn.push_back('0');
		std::cout << Kn << i+1 << " =\t" << BINVECT_out(keys[i], 6) << "\n";
	}
	std::cout << "\n\n";

	std::cout << "MESSAGE (IP):\n " << BINVECT_out(message, 32) << "\n\n";


	auto messageLR = VbinTo2Vbin(message);
	auto L = messageLR.first;
	auto R = messageLR.second;
	BINEVECTOR LPrev;
	BINEVECTOR RPrev;

	unsigned raund_i = 1;
	for (auto i : keys)
	{
		std::cout << "___RAUND_" << raund_i << "_______" << "\n";
		LPrev = L; // LPrev = L(i-1)
		RPrev = R; // RPrev = R(i-1)

		auto f = F_RAUND(RPrev, i);
		R = xor_binvector(LPrev, f);
		L = RPrev;
		
		std::cout << "L" << raund_i - 1 << ":\t\t    " << BINVECT_out(LPrev, 8) << "\n";
		std::cout << "R" << raund_i-1 << ":\t\t    " << BINVECT_out(RPrev, 8) << "\n\n";
		std::cout << "F" << raund_i << ":\t\t    " << BINVECT_out(f, 8) << "\n";
		std::cout << "L" << raund_i << " = R" << raund_i - 1 << ":\t    " << BINVECT_out(L, 8) << "\n";
		std::cout << "R" << raund_i << " = XOR(L"<< raund_i - 1 << ",F"<< raund_i <<")"  << ": " << BINVECT_out(R, 8) << "\n";
		

		std::cout << "___END__RAUND_" << raund_i << "_______" << "\n\n\n";
		raund_i++;
	}

	std::cout << "END CRYPTO MESSAGE :\n " << BINVECT_out(V2binToOne(L, R), 8) << "\n\n";
	std::cout << "END CRYPTO MESSAGE RIGHT LEFT MODE :\n " << BINVECT_out(V2binToOne(R, L), 8) << "\n\n";

	message = permutation(V2binToOne(R, L), PERMUTATION_END_MODE);
	
	std::cout << "END CRYPTO MESSAGE RIGHT LEFT MODE (IP^(-1)):\n " << BINVECT_out(message, 8) << "\n\n";

	std::cout << "MESSAGE: " << BINVECT_out(message64, 8) << "\n";
	std::cout << "KEY    : " << BINVECT_out(key64, 8) << "\n";
	std::cout << "CRYPTO : " << BINVECT_out(message, 8) << "\n";
	return message;
}

bool test_DES64()
{
	std::string M = "00000001 00100011 01000101 01100111 10001001 10101011 11001101 11101111";
	std::string K = "00010011 00110100 01010111 01111001 10011011 10111100 11011111 11110001";
	std::string O = "10000101 11101000 00010011 01010100 00001111 00001010 10110100 00000101";

	return stringToBytes(O) == DES64(stringToBytes(M), stringToBytes(K));
}

void colorTest(std::string name,bool b)
{
	std::cout << name << " :  "; 

	if(b)
		std::cout << GREEN << "[SUCCESS]" << RESET;
	else
		std::cout << RED   << "[FAILING]" << RESET;

	std::cout << std::endl;
}
void TEST()
{
	colorTest("stringToBytes ", test_strToByts());
	colorTest("permutation start", test_permutation_start());
	colorTest("permutation end ", test_permutation_end());
	//colorTest("key 64 to 56     ", test_keyconvert());

	colorTest("make 2 keys from 64 ", test_make_pair_keys());
	colorTest("left shift ", test_leftshift());
	colorTest("double left shift ", test_2shift());
	colorTest("pair to one", test_pairtoone());
	colorTest("compress raun key", test_compresKey());
	colorTest("test raund keys genegation", test_genation16raundKeys());
	colorTest("test extert 32 bits to 48 (e)", test_fundtionE());
	colorTest("test xor", test_binXor());
	colorTest("test xor bin vector", test_xor_binvector());
	//adhfsdf

	colorTest("test Sblock item", test_SblockItem());
	colorTest("test Unsingle to bin convert", test_DecToBin4());
	colorTest("test Bin to Dec convert", test_Bin4toDec());
	colorTest("test convert 6 bit to 4 bit with STable", test_Bit6toSblockto4bit());
	colorTest("test VbinTo2Vbin", test_VbinTo2Vbin());
	colorTest("test V2binToOne", test_V2binToOne());
	colorTest("test 48 to 8x6bit", test_Vector42to8x6());
	colorTest("test F Raund", test_F_RAUND());
	colorTest("test P table", test_fundtionP());
	colorTest("test DES 64", test_DES64());
}

bool isValidKey(std::string key)
{
	if (key.size() == 64)
		return true;
	return false;
}

BINEVECTOR AsciiToBinVect(std::string message);
BINEVECTOR KeyGenerator();

void USER_IO()
{
	std::string message_bin_raw;
	std::string key_raw;

	std::cout << "Input you message (64 bin): "; 
	std::cin >> message_bin_raw;

	char isNewKey;
	std::cout << "Generate new key (Y/n)? ";
	std::cin >> isNewKey;
	if (isNewKey == 'n')
	{

		do {
			std::cout << "Input key (64 of BASE64): ";
			std::cin >> key_raw;
		} while (!isValidKey(key_raw));
	}

	auto result = DES64(stringToBytes(message_bin_raw), stringToBytes(key_raw));

	for (auto i : result)
		std::cout << i;

	std::cout << std::endl;
}

std::string BINVECT_out(BINEVECTOR vect, unsigned mode)
{
	std::string out;
	for (unsigned i = 0; i < vect.size(); ++i)
	{
		if (i % mode == 0 && i != 0)
			out.push_back(' ');
		
		if (vect[i] == 0)
			out.push_back('0');
		else
			out.push_back('1');
	}
	return out;
}
int main()
{
	//TEST();
	//USER_IO();

	std::string message_bin_raw = "01001111 01101110 01100101 00100000 01110011 01101000 01101111 01110100";
	std::string key_raw =		  "01101111 01101110 01100101 00100000 01101011 01101001 01101100 01101100";
							//OUT:"00010100 01101011 01100111 01001011 00010101 11110000 00000001 10000010"
	
	DES64(stringToBytes(message_bin_raw), stringToBytes(key_raw));
	//std::cout << BINVECT_out(, 8);
	return 0;
}
