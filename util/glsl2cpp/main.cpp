/*
 * Copyright (c) 2015 Daniel Kirchner
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <lz4.h>
#include <lz4hc.h>
#include <stdint.h>
#include <iomanip>
#include <sstream>

std::string inputfilename;
std::string outputfilename;
std::string id;
std::vector<std::string> incdirs;
std::vector<std::string> headers;
std::vector<std::string> append;
std::vector<std::string> prepend;

#ifdef _WIN32
#define DIR_SEPARATOR '\\'
#else
#define DIR_SEPARATOR '/'
#endif

const char *typename32 = "uint32_t";
const char *typename8 = "uint8_t";
const char *structname = NULL;

void add_incdir (const std::string &dir)
{
	incdirs.push_back (dir);
	if (incdirs.back ()[incdirs.back ().length () - 1] != DIR_SEPARATOR)
	{
		incdirs.back () += DIR_SEPARATOR;
	}
}

void usage (const char *progname)
{
	std::cerr << "Usage: " << progname << " [options] [id] [input] [output]"
						<< std::endl
						<< "Packs a GLSL source file into c++ code." << std::endl
						<< std::endl
						<< "  -H    header file to include in the output file"
						<< std::endl
						<< "        (including <> or \"\")" << std::endl
						<< "  -P    a line to prepend to the output file" << std::endl
						<< "  -A    a line to append to the output file" << std::endl
						<< "  -S    the name for a struct to pack the data into"
						<< std::endl
						<< "  -I    specifies an include directory" << std::endl
						<< "  -T8   specifies the unsigned 8-bit data type to use"
						<< std::endl
						<< "  -T32  specifies the unsigned 32-bit data type to use"
						<< std::endl
						<< "        (this is ignored if -S is used)" << std::endl;
}

int parse_args (int argc, char *argv[])
{
	int parsed = 0;
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'P':
				if (argv[i][2] != 0)
				{
					std::cerr << "Invalid option: " << argv[i] << std::endl;
					return -1;
				}
				i++;
				if (i >= argc)
				{
					std::cerr << "No expression specified after -P."
										<< std::endl;
					return -1;
				}
				prepend.push_back (argv[i]);
				continue;
			case 'A':
				if (argv[i][2] != 0)
				{
					std::cerr << "Invalid option: " << argv[i] << std::endl;
					return -1;
				}
				i++;
				if (i >= argc)
				{
					std::cerr << "No expression specified after -A."
										<< std::endl;
					return -1;
				}
				append.push_back (argv[i]);
				continue;
			case 'S':
				if (argv[i][2] != 0)
				{
					std::cerr << "Invalid option: " << argv[i] << std::endl;
					return -1;
				}
				i++;
				if (i >= argc)
				{
					std::cerr << "No structure name specified after -S."
										<< std::endl;
					return -1;
				}
				structname = argv[i];
				continue;
			case 'H':
				if (argv[i][2] != 0)
				{
					std::cerr << "Invalid option: " << argv[i] << std::endl;
					return -1;
				}
				i++;
				if (i >= argc)
				{
					std::cerr << "No header specified after -H."
										<< std::endl;
					return -1;
				}
				headers.push_back (argv[i]);
				continue;
			case 'I':
				if (argv[i][2] == 0)
				{
					i++;
					if (i >= argc)
					{
						std::cerr << "No include directory specified after -I."
											<< std::endl;
						return -1;
					}
					add_incdir (argv[i]);
					continue;
				}
				add_incdir (&argv[i][2]);
				continue;
			case 'T':
				if (argv[i][2] == '3')
				{
					if (argv[i][3] == '2')
					{
						if (argv[i][4] == 0)
						{
							i++;
							if (i >= argc)
							{
								std::cerr  << "No type name specified after -T32."
													 << std::endl;
								return -1;
							}
							typename32 = argv[i];
							continue;
						}
					}
				}
				else if (argv[i][2] == '8')
				{
					if (argv[i][3] == 0)
					{
						i++;
						if (i >= argc)
						{
							std::cerr  << "No type name specified after -T8."
												 << std::endl;
							return -1;
						}
						typename8 = argv[i];
						continue;
					}
				}
				continue;
			default:
				std::cerr << "Invalid option: " << argv[i] << std::endl;
				return -1;
			}
		}

		switch (parsed++)
		{
		case 0:
			id = argv[i];
			break;
		case 1:
			inputfilename = argv[i];
			break;
		case 2:
			outputfilename = argv[i];
			break;
		default:
			std::cerr << "Too many arguments." << std::endl;
			return -1;
		}
	}

	if (parsed < 3)
	{
		usage (argv[0]);
		return -1;
	}

	return 0;
}

bool read_file (const std::string &filename, std::vector<char> &data,
								bool searchinc)
{
	std::ifstream in (filename.c_str (), std::ios_base::in);
	if (!in.is_open ())
	{
		if (searchinc)
		{
			for (std::vector<std::string>::iterator it = incdirs.begin ();
					 it != incdirs.end (); it++)
			{
				std::string name;
				name = (*it) + filename;
				in.open (name.c_str (), std::ios_base::in);
				if (in.is_open ())
					 break;
			}
		}
		if (!in.is_open ())
		{
			std::cerr << "Cannot open " << filename << "." << std::endl;
			return false;
		}
	}
	while (in.good ())
	{
		char c = in.get ();
		if (in.good ())
			 data.push_back (c);
	}
	in.close ();
	return true;
}

unsigned int source_string_number = 0;

bool parse_file (const std::string &filename, std::string &version, std::string &data,
								 unsigned int my_source_string_number, bool searchinc = false)
{
	bool comment = false;
	unsigned int commentstart = 0;
	unsigned int line = 1;
	std::vector<char> input;
	if (!read_file (filename, input, searchinc))
		 return false;

	{
		std::ostringstream stream;
		stream << "#line " << line << " "
					 << my_source_string_number << std::endl;
		data.append (stream.str ());
	}

	bool newline = true;
	for (size_t i = 0; i < input.size (); i++)
	{
		if (!comment)
		{
			// check for #version directive
			if (newline && i + 9 < input.size () && !strncmp (&input[i], "#version ", 9)) {
				while (i < input.size () && input[i] != '\n')
				{
					version += input[i];
					i++;
				}
				version += "\n";
				continue;
			}
			// check for #include directive
			else if (newline && i + 9 < input.size ()
					&& !strncmp (&input[i], "#include ", 9))
			{
				i += 9;
				std::string includefile;
				while (i < input.size () && input[i] != '\n')
				{
					includefile += input[i];
					i++;
				}
				while (isspace (includefile[0]) || includefile[0] == '\"'
							 || includefile[0] == '<')
					 includefile.erase (0, 1);
				while (isspace (includefile[includefile.length () - 1])
							 || includefile[includefile.length () - 1] == '\"'
							 || includefile[includefile.length () - 1] == '>')
					 includefile.erase (includefile.length () - 1);
				if (!parse_file (includefile, version, data, source_string_number++, true))
					 return false;
				
				{
					std::ostringstream stream;
					line++;
					stream << "#line " << line << " "
								 << my_source_string_number << std::endl;
					data.append (stream.str ());
				}
				continue;
			}
			if (i + 1 < input.size () && input[i] == '/')
			{
				if (input[i + 1] == '*')
				{
					comment = true;
					if (!newline)
						 data.push_back ('\n');
					commentstart = line;
				}
				else if (input[i + 1] == '/')
				{
					while (input[i + 1] != '\n' && i + 1 < input.size ())
						 i++;
					continue;
				}
			}


		}
		else
		{
			if (i + 1 < input.size () && input[i] == '*'
					&& input[i + 1] == '/')
			{
				comment = false;
				if (commentstart != line)
				{
					std::ostringstream stream;
					line++;
					stream << "#line " << line << " "
								 << my_source_string_number << std::endl;
					data.append (stream.str ());
				}
				i += 2;
				newline = true;
				continue;
			}
		}

		if (!isspace (input[i]))
			 newline = false;
		else if (input[i] == '\n')
		{
			line++;
			newline = true;
		}

		if (!comment)
			 data.push_back (input[i]);
	}

	return true;
}

int main (int argc, char *argv[])
{
	if (parse_args (argc, argv))
		 return -1;

	std::ofstream out (outputfilename.c_str (),
										 std::ios_base::out|std::ios_base::trunc);
	if (!out.is_open ())
	{
		std::cerr << "Cannot open " << outputfilename << std::endl;
		return -1;
	}

	std::string version;
	std::string data;

	if (!parse_file (inputfilename, version, data, source_string_number++, false))
		 return -1;

	data.insert (data.begin (), version.begin (), version.end ());

	std::vector<unsigned char> output;
	output.resize (LZ4_compressBound (data.size ()));

	int len = LZ4_compressHC2 (&data[0], reinterpret_cast<char*> (&output[0]),
														data.size (), 16);

	out << "/* this file was generated by glsl2cpp" << std::endl
			<< " * do not attempt to edit it directly */" << std::endl;

	for (std::vector<std::string>::iterator it = headers.begin ();
			 it != headers.end (); it++)
	{
		out << "#include " << *it << std::endl;
	}
	out << std::endl;

	for (std::vector<std::string>::iterator it = prepend.begin ();
			 it != prepend.end (); it++)
	{
		out << (*it) << std::endl;
	}
	if (!prepend.empty ())
		 out << std::endl;

	if (structname != NULL)
	{
		out << "const struct " << structname << " " << id << " = { "
				<< data.size () << ", (const " << typename8 << "[]) {" << std::endl;
	}
	else
	{
		out << typename32 << " " << id << "_length = "
				<< std::dec << data.size () << ";" << std::endl
				<< "const " << typename8 << " " << id << "_data[] = {" << std::endl;
	}
	int i = 0;
	while (len - i > 8)
	{
		out << "    ";
		for (int c = 0; c < 7; c++)
		{
			out << "0x" << std::hex << std::setfill ('0')
					<< std::setw (2) << (unsigned int) (output[i++])
					<< std::dec << ", ";
		}
		out << "0x" << std::hex << std::setfill ('0')
				<< std::setw (2) << (unsigned int) (output[i++])
				<< std::dec << "," << std::endl;
	}
	out << "    ";
	for (; i < len - 1; i++)
	{
		out << "0x" << std::hex << std::setfill ('0')
				<< std::setw (2) << (unsigned int) (output[i])
				<< std::dec << ", ";
	}
	out << "0x" << std::hex << std::setfill ('0')
			<< std::setw (2) << (unsigned int) (output[i]) << std::endl;
	if (structname != NULL)
	{
		out << "}";
	}
	out << "};" << std::endl;

	if (!append.empty ())
		 out << std::endl;

	for (std::vector<std::string>::iterator it = append.begin ();
			 it != append.end (); it++)
	{
		out << (*it) << std::endl;
	}

	return 0;
}
