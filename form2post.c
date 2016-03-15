#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <libxml/xinclude.h>
#include <libxml/HTMLparser.h>

/**
 * Find an XML tag by it's name within a given XML node.
 *
 * @param scope
 * 	The XML node to search in.
 * @param name
 * 	The name of the XML tag to find.
 * @param index
 * 	The number of tags to skip before returning a valid node. This value may
 * 	change as the function progresses.
 * @return
 * 	An xmlNodePtr if found. If no tag with the given type is found, this will be
 * 	NULL.
 */
xmlNodePtr _getTag(xmlNodePtr scope, const char *name, size_t *index)
{
	xmlNodePtr child = NULL, curr = NULL;

	if(scope == NULL)
	{
		return NULL;
	}

	child = scope->children;
	while(child != NULL)
	{
		if(xmlStrcmp(child->name, (xmlChar*) name) == 0 && *index == 0)
		{
			return child;
		}
		else
		{
			if(xmlStrcmp(child->name, (xmlChar*) name) == 0)
			{
				--(*index);
			}
			curr = _getTag(child, name, index);
			if(curr != NULL && *index == 0)
			{
				return curr;
			}
		}
		child = child->next;
	}
	return NULL;
}

/**
 * Find an attribute of a given node.
 *
 * @param input
 * 	The node to check for an attribute with in.
 * @param name
 * 	The attribute name to loop for.
 * @return
 * 	The attribute is found, this is the null terminated string containing it's
 * 	value. If it can't be found, this is NULL.
 */
const char* getAttribute(xmlNodePtr input, const char* name)
{
	if(input == NULL)
	{
		return NULL;
	}

	xmlAttrPtr property = input->properties;
	while(property != NULL)
	{
		if(property->children != NULL)
		{
			if(xmlStrcmp(property->name, (xmlChar*) name) == 0)
			{
				return (const char*) property->children->content;
			}
		}
		property = property->next;
	}
	return NULL;
}

/**
 * A small wrapper for the _getTag that doesn't modify the index parameter.
 * @see _getTag
 */
xmlNodePtr getTag(xmlNodePtr scope, const char* name, size_t index)
{
	return _getTag(scope, name, &index);
}

int main(int argc, char** argv)
{
	htmlDocPtr doc = NULL;
	xmlNodePtr root = NULL, form = NULL, input = NULL;
	size_t inputIndex = 0, formIndex = 0, i = 0;
	char tmpFileName[31] = "/tmp/form2post/";
	const char *formAction, *formMethod, *inputName, *inputValue;
	char tmpChar;
	FILE* tmpFile;

	struct stat st = {0};

	if(stat(tmpFileName, &st) == -1)
	{
		mkdir(tmpFileName, 0777);
	}

	// Generate random tmp file
	srand(time(NULL));
	for(i = 15; i < 30; ++i)
	{
		tmpFileName[i] = rand() % 26 + 97;
	}

	// Write stdin to tmp file
	tmpFile = fopen(tmpFileName, "w+");
	while((tmpChar = getchar()) != EOF)
	{
		fputc(tmpChar, tmpFile);
	}
	fclose(tmpFile);

	// Open HTML file in XML parser
	doc = htmlParseFile(tmpFileName, "UTF-8");
	if(doc == NULL)
	{
		fprintf(stderr, "Failed to parse file\n");
		return 1;
	}

	// Get XML root element
	root = xmlDocGetRootElement(doc);
	if(root == NULL)
	{
		fprintf(stderr, "This file contains no valid root element\n");
	}

	// Loop through all "form" tags
	while((form = getTag(root, "form", formIndex++)) != NULL)
	{
		formAction = getAttribute(form, "action");
		formMethod = getAttribute(form, "method");
		if(formMethod != NULL)
		{
			char tmp[strlen(formMethod) + 1];
			while((tmp[i] = toupper(formMethod[i]))) { ++i; }
			if(strncmp(tmp, "POST", 5))
			{
				formMethod = "POST";
			}
			else
			{
				formMethod = "GET";
			}
		}
		else
		{
			formMethod = "GET";
		}

		if(formAction == NULL)
		{
			formAction = "";
		}
		printf("%s %s", formMethod, formAction);

		// Loop through each input of the current form
		inputIndex = 0;
		input = getTag(form, "input", inputIndex++);
		while(input != NULL)
		{
			inputName = getAttribute(input, "name");
			inputValue = getAttribute(input, "value");

			if(inputName != NULL)
			{
				if(inputValue == NULL)
				{
					inputValue = "";
				}
				printf(inputIndex == 1 ? "?" : "&");
				printf("%s=%s", inputName, inputValue);
			}
			input = getTag(form, "input", inputIndex++);
		}
		printf("\n");
	}
	remove(tmpFileName);
	return 0;
}
