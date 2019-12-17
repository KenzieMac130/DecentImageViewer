#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define CUTE_FILES_IMPLEMENTATION
#include "cute_files.h"

struct ShaderGenContext
{
	FILE* header;
	FILE* implimentation;
};

void OnExit(void)
{
	_fcloseall();
}

void openFile(FILE** fp, const char* path, const char* mode)
{
	errno_t error;
	error = fopen_s(fp, path, mode);
	if(error)
	{
		printf("FILE ERROR! %s: %s", strerror(error), path);
		exit(-1);
	}
}

void fileNameToEnumName(char* buffer, size_t bufferLength)
{
	size_t remainingSpace = bufferLength;
	char* secondBuffer[256];
	char* parsePos = buffer;

	/*Itterate through each character*/
	while (*parsePos && remainingSpace)
	{
		remainingSpace--;

		/*Remove Junk*/
		if (isspace(*parsePos) || !isalnum(*parsePos))
		{
			*parsePos = '_';
		}
		
		/*Add Underscores*/
		if ((isupper(*parsePos) || isdigit(*parsePos)) && remainingSpace - 1 > 0)
		{
			/*Move over and add "_"*/
			strncpy_s(secondBuffer, 512, parsePos, remainingSpace);
			strncpy_s(parsePos + 1, remainingSpace - 1, secondBuffer, 256);
			
			*parsePos = '_';
			parsePos++;
		}

		/*Terminate after dot*/
		if (*parsePos == '.')
		{
			*parsePos = '\0';
			break;
		}

		/*UpperCase*/
		*parsePos = toupper(*parsePos);

		parsePos++;
	}
	buffer[bufferLength - 1] = '\0';
}

void processGLSLFile(cf_file_t* file, void* udata)
{
	struct ShaderGenContext* ctx = (struct ShaderGenContext*)udata;

	if (cf_match_ext(file, "glsl"))
	{
		return;
	}
	printf("Found GLSL File: %s\n", file->path);

	FILE* shaderFile;
	openFile(&shaderFile, file->path, "r");

	char* contents;
	size_t contentSize;

	/*Get File Size*/
	fseek(shaderFile, 0, SEEK_END);
	contentSize = ftell(shaderFile);
	fseek(shaderFile, 0, SEEK_SET);

	contents = (char*)malloc(contentSize);

	fread_s(contents, contentSize, 1, contentSize, shaderFile);

	/*Write contents*/
	fprintf_s(ctx->implimentation, "\t\"%.*s\",\n", (int)contentSize, contents);

	/*Generate and Write Enum Name*/
	char enumName[256];
	strncpy_s(enumName, 256, file->name, 256);
	fileNameToEnumName(enumName, 256);
	fprintf_s(ctx->header, "\tGEN_BUILTIN_SHADER_%s,\n", enumName);

	fclose(shaderFile);
}

/*Args: 
1: Shader Mode
2:Implimentation
3:Header
4:Shader Directory*/
int main(int argc, char* argv[])
{
	atexit(OnExit);
	assert(argc == 4);
	const char* mode = argv[1];
	const char* implimentationPath = argv[2];
	const char* headerPath = argv[3];
	const char* shaderDirectory = argv[4];

	printf("Running Shader Implementation Generator\n"
			"Mode: %s\n"
			"Implimentation: %s\n"
			"Header: %s\n"
			"Shader Directory: %s\n", 
			mode,
			implimentationPath,
			headerPath, 
			shaderDirectory);
	assert(cf_file_exists(shaderDirectory));

	/*Open Header and Implimentation File for Writing*/
	struct ShaderGenContext ctx;
	openFile(&ctx.header, headerPath, "w");
	openFile(&ctx.implimentation, implimentationPath, "w");

	/*Write beginning of Header*/
	fputs("/*Generated Shader Collection*/\n\n"
		"#pragma once\n"
		"#ifdef __cplusplus\n"
		"extern \"C\" {\n"
		"#endif\n"
		"#include <stdint.h>\n\n"
		"typedef enum\n{\n", ctx.header);

	/*Write beginning of Implimentation*/
	fputs("/*Generated Shader Collection*/\n\n"
		"#include\"gen_ShaderImplementation.h\"\n\n"
		"const char* gen_BuiltinShaders_Code[] =\n{\n", ctx.implimentation);

	/*Process all GLSL Files*/
	cf_traverse(shaderDirectory, processGLSLFile, &ctx);

	/*Write end of Header*/
	fputs("\tGEN_BUILTIN_SHADER_COUNT,\n"
		"\tGEN_BUILTIN_SHADER_MAX = UINT32_MAX\n"
		"} gen_BuiltinShaderTypes;\n\n"
		"const char* gen_BuiltinShaders_Code[GEN_BUILTIN_SHADER_COUNT];\n\n"
		"#ifdef __cplusplus\n"
		"}\n\n"
		"#endif", ctx.header);

	/*Write end of Implimentation*/
	fputs("};", ctx.implimentation);

	fclose(ctx.header);
	fclose(ctx.implimentation);

	return 0;
}