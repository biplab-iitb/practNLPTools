
# encoding: utf-8
# Practical Natural Language Processing Tools (practNLPTools-lite):
#               Combination of Senna and Stanford dependency Extractor
# Copyright (C) 2014 PractNLP Project
# Original Author: Biplab Ch Das' <bipla12@cse.iitb.ac.in>
# Current Author: Jawahar S <jawahar273@gmail.com>
# URL: <http://www.cse.iitb.ac.in/biplab12>
# URL: http://jawahar273.gitbooks.io (or) http://github.com/jawahar273


from __future__ import generators, print_function, unicode_literals
import subprocess

import os
from platform import architecture, system

class Annotator:
    """A general interface of the SENNA/Stanford Dependency Extractor pipeline that supports any of
    the operations specified in SUPPORTED_OPERATIONS.
    SUPPORTED_OPERATIONS: It provides
    Part of Speech Tags, Semantic Role Labels, Shallow Parsing (Chunking),
    Named Entity Recognisation (NER), Dependency Parse and
    Syntactic Constituency Parse.
    Applying multiple operations at once has the speed advantage. For example,
    senna v3.0 will calculate the POS tags in case you are extracting the named
    entities. Applying both of the operations will cost only the time of
    extracting the named entities. Same is true for dependency Parsing.
    SENNA pipeline has a fixed maximum size of the sentences that it can read.
    By default it is 1024 token/sentence. If you have larger sentences, changing
    the MAX_SENTENCE_SIZE value in SENNA_main.c should be considered and your
    system specific binary should be rebuilt. Otherwise this could introduce
    misalignment errors
    and for Dependency Parser the requirement is Java Runtime Environment :)

    :param str senna_path: path where is located
    :param str dep_model: Stanford dependencie mode
    :param str jar_path: path of stanford parser jar
    """

    def __init__(self, senna_path="", dep_model="", jar_path=""):
        """
        init function of Annotator class
        """
        self.senna_path = ""
        self.dep_par_path = None
        self.dep_par_model = None
        jar_path = False
        if jar_path:
            self.dep_par_path = jar_path+os.path.sep
        else:
            self.dep_par_path = os.getcwd()+os.path.sep

        if dep_model:
            self.dep_par_model = dep_model
        else:
            self.dep_par_model = 'edu.stanford.nlp.trees.EnglishGrammaticalStructure'

        self.default_jar_cli = ['java', '-cp', 'stanford-parser.jar',\
                        self.dep_par_model, \
                      '-treeFile', 'in.parse', '-collapsed']

        self.print_values()


    def print_values(self):
        """displays the current set of values such as SENNA location, stanford parser jar,
          jar command interface
        """
        print("*"*50)
        print("default values:\nsenna path:\n", self.senna_path, \
             "\nDependencie parser:\n", self.dep_par_path)
        print("Stanford parser clr", " ".join(self.default_jar_cli))
        print("*"*50)

    def check_stp_jar(self, path, raise_exp=False):
        """Check the stanford parser is present in the given directions 
        and nested searching will be added in futurwork

        :param str path: path of where the stanford parser is present
        :param bool raise_exp: to raise exception with user wise and default `False`
              don't raises exception
        :return: given path if it is valid one or return boolean `False` or
             if raise FileNotFoundError on raise_exp=True
        :rtype: bool

        """
        path = os.listdir(path)
        file_found = False
        for file in path:
            if file.endwith(".jar"):
                if file.startwith("stanford-parser"):
                    file_found = True
        if not file_found and raise_exp:
            raise FileNotFoundError("`stanford-parser.jar` is not found in the given path")
        return file_found

    @property
    def stp_dir(self):
        """The return the path of stanford parser jar location
        and set the path for Dependency Parse at run time(this is python @property)
        """
        return self.dep_par_path

    @stp_dir.setter
    def stp_dir(self, val):
        if os.path.isdir(val):
            self.dep_par_path = val+os.path.sep


    @property
    def senna_dir(self):
        """The return the path of senna location
        and set the path for senna at run time(this is python @property)
        :rtype: string
        """
        return self.senna_path

    @senna_dir.setter
    def senna_dir(self, val):
        if os.path.isdir(val):
            self.senna_path = val+os.path.sep


    @property
    def jar_cli(self):
        """The return cli for standford-parser.jar(this is python @property)

        :rtype: string
        """
        return " ".join(self.default_jar_cli)

    @jar_cli.setter
    def jar_cli(self, val):
        self.default_jar_cli = val.split()

    def get_cos_name(self, os_name):
        """get the executable binary with respect to the Os name.

        :param str os_name: os name like Linux, Darwin, Windows
        :return: the corresponding exceutable object file of senna
        :rtype: str
        """

        if os_name == 'Linux':
            bits = architecture()[0]
            if bits == '64bit':
                executable = 'senna-linux64'
            elif bits == '32bit':
                executable = 'senna-linux32'
            else:
                executable = 'senna'
        elif os_name == 'Darwin':
            executable = 'senna-osx'
        elif os_name == 'Windows':
            executable = 'senna-win32.exe'
        return self.senna_path+executable

    def getSennaTagBatch(self, sentences):
        """Communicates with senna through lower level communiction(sub process)
        and converted the console output(default is file writing).
        On batch processing each end is add with new line.

        :param list sentences: list of sentences for batch processes
        :rtype: str
        """
        input_data = ""
        for sentence in sentences:
            input_data += sentence+"\n"
        input_data = input_data[:-1]
        package_directory = os.path.dirname(self.senna_path)
        os_name = system()
        executable = self.get_cos_name(os_name)
        senna_executable = os.path.join(package_directory, executable)
        cwd = os.getcwd()
        os.chdir(package_directory)
        pipe = subprocess.Popen(senna_executable, stdout=subprocess.PIPE, stdin=subprocess.PIPE)
        senna_stdout = pipe.communicate(input=input_data.encode('utf-8'))[0]
        os.chdir(cwd)
        return senna_stdout.decode().split("\n\n")[0:-1]

    def getSennaTag(self, sentence):
        """Communicates with senna through lower level communiction(sub process)
        and converted the console output(default is file writing)

        :param str or listsentences: list of sentences for batch processes
        :return: senna tagged output
        :rtype: str
        """
        input_data = sentence
        package_directory = os.path.dirname(self.senna_path)
        #print("testing dir",self.dep_par_path, package_directory)
        os_name = system()
        executable = self.get_cos_name(os_name)
        senna_executable = os.path.join(package_directory, executable)
        cwd = os.getcwd()
        os.chdir(package_directory)
        pipe = subprocess.Popen(senna_executable, stdout=subprocess.PIPE, stdin=subprocess.PIPE)
        senna_stdout = pipe.communicate(input=" ".join(input_data).encode('utf-8'))[0]
        os.chdir(cwd)
        return senna_stdout

    def getDependency(self, parse):
        """Change to the Stanford parser direction and process the works

        :param str parse: parse is the input(tree format) and it is writen in as file

        :return: stanford dependency universal format
        :rtype: str
        """
        #print("\nrunning.........")
        package_directory = os.path.dirname(self.dep_par_path)
        cwd = os.getcwd()
        os.chdir(package_directory)
        with open("in.parse", "w", encoding='utf-8') as parsefile:
            parsefile.write(parse)
        pipe = subprocess.Popen(self.default_jar_cli, stdout=subprocess.PIPE, \
             stderr=subprocess.PIPE)
        pipe.wait()
        stanford_out = pipe.stdout.read()
        os.chdir(cwd)
        return stanford_out.decode("utf-8").strip()

    def getBatchAnnotations(self, sentences, dep_parse=True):
        """
          :param list sentences: list of sentences
          :rtype: dict
        """
        annotations = []
        batch_senna_tags = self.getSennaTagBatch(sentences)
        for senna_tags in batch_senna_tags:
            annotations += [self.getAnnotations(senna_tags=senna_tags)]
        if dep_parse:
            syntax_tree = ""
            for annotation in annotations:
                syntax_tree += annotation['syntax_tree']
            dependencies = self.getDependency(syntax_tree).split("\n\n")
            #print dependencies
            if len(annotations) == len(dependencies):
                for dependencie, annotation in zip(dependencies, annotations):
                    annotation["dep_parse"] = dependencie
        return annotations


    def getAnnotations(self, sentence="", senna_tags=None, batch=False, dep_parse=True):
        """passing the string to senna and performing aboue given nlp process
        and the returning them in a form of `dict()`

        :parama str or list sentence: a sentence or list of sentence for nlp process.
        :parama str or list senna_tags:  this values are by SENNA processed string
        :parama bool  batch: the change the mode into batch processing process
        :param bool dep_parse: to tell the code and user need to communicate with stanford parser
        :return: the dict() of every out in the process such as ner, dep_parse, srl, verbs etc.
        :rtype: dict
        """
        annotations = {}
        if not senna_tags:
            senna_tags = self.getSennaTag(sentence).decode()
            senna_tags = [x.strip() for x in senna_tags.split("\n")];senna_tags = senna_tags[0:-2]
        else:
            senna_tags = [x.strip() for x in senna_tags.split("\n")]
        no_verbs = len(senna_tags[0].split("\t"))-6

        words = [];pos = [];chunk = [];ner = [];verb = [];srls = [];syn = []
        for senna_tag in senna_tags:
            senna_tag = senna_tag.split("\t")
            words += [senna_tag[0].strip()]
            pos += [senna_tag[1].strip()]
            chunk += [senna_tag[2].strip()]
            ner += [senna_tag[3].strip()]
            verb += [senna_tag[4].strip()]
            srl = []
            for i in range(5, 5+no_verbs):
                srl += [senna_tag[i].strip()]
            srls += [tuple(srl)]
            syn += [senna_tag[-1]]
        roles = []
        for j in range(no_verbs):
            role = {}
            i = 0
            temp = ""
            curr_labels = [x[j] for x in srls]
            for curr_label in curr_labels:
                splits = curr_label.split("-")
                if splits[0] == "S":
                    if len(splits) == 2:
                        if splits[1] == "V":
                            role[splits[1]] = words[i]
                        else:
                            if splits[1] in role:
                                role[splits[1]] += " "+words[i]
                            else:
                                role[splits[1]] = words[i]
                    elif len(splits) == 3:
                        if splits[1]+"-"+splits[2] in role:
                            role[splits[1]+"-"+splits[2]] += " "+words[i]
                        else:
                            role[splits[1]+"-"+splits[2]] = words[i]
                elif splits[0] == "B":
                    temp = temp+" "+words[i]
                elif splits[0] == "I":
                    temp = temp+" "+words[i]
                elif splits[0] == "E":
                    temp = temp+" "+words[i]
                    if len(splits) == 2:
                        if splits[1] == "V":
                            role[splits[1]] = temp.strip()
                        else:
                            if splits[1] in role:
                                role[splits[1]] += " "+temp
                                role[splits[1]] = role[splits[1]].strip()
                            else:
                                role[splits[1]] = temp.strip()
                    elif len(splits) == 3:
                        if splits[1]+"-"+splits[2] in role:
                            role[splits[1]+"-"+splits[2]] += " "+temp
                            role[splits[1]+"-"+splits[2]] = role[splits[1]+"-"+splits[2]].strip()
                        else:
                            role[splits[1]+"-"+splits[2]] = temp.strip()
                    temp = ""
                i += 1
            if "V" in role:
                roles += [role]
        annotations['words'] = words
        annotations['pos'] = list(zip(words, pos))
        annotations['ner'] = list(zip(words, ner))
        annotations['srl'] = roles
        annotations['verbs'] = [x for x in verb if x != "-"]
        annotations['chunk'] = list(zip(words, chunk))
        annotations['dep_parse'] = ""
        annotations['syntax_tree'] = ""
        for (word_, syn_, pos_) in zip(words, syn, pos):
            annotations['syntax_tree'] += syn_.replace("*", "("+pos_+" "+word_+")")
        #annotations['syntax_tree']=annotations['syntax_tree'].replace("S1","S")
        if dep_parse:
            annotations['dep_parse'] = self.getDependency(annotations['syntax_tree'])
        return annotations


def test(senna_path="/media/jawahar/jon/ubuntu/senna", sent="", dep_model="", batch=False, 
               jar_path="/media/jawahar/jon/ubuntu/practNLPTools-lite/pntl"):
    """please replace the path of yours environment(accouding to OS path)

    :parama str senna_path: path for senna location
    :parama str dep_model: stanford dependency parser model location
    :parama str or list sent: the sentense to process with Senna
    :parama bool batch: makeing as batch process with one or more sentense passing
    :parama str jar_path: location of stanford-parser.jar file
    """
    senna_path=""
    jar_path=""
    #from pntl.utils import skipgrams
    annotator = Annotator(senna_path, dep_model, jar_path)
    if not sent:
        if not batch:
            sent = "He created the robot and broke it after making it."
            print("\n", sent, "\n")
            sent = sent.split()
            print('dep_parse:\n', (annotator.getAnnotations(sent, dep_parse=True)['dep_parse']))
            print('chunk:\n', (annotator.getAnnotations(sent, dep_parse=True)['chunk']))
            print('pos:\n', (annotator.getAnnotations(sent, dep_parse=True)['pos']))
            print('ner:\n', (annotator.getAnnotations(sent, dep_parse=True)['ner']))
            print('srl:\n', (annotator.getAnnotations(sent, dep_parse=True)['srl']))
            print('syntax tree:\n', (annotator.getAnnotations(sent, dep_parse=True)['syntax_tree']))
            print('words:\n', (annotator.getAnnotations(sent, dep_parse=True)['words']))
            #print('skip gram\n', list(skipgrams(sent, n=3, k=2)))
        else:
            sent = ["He killed the man with a knife and murdered him with a dagger.",\
                "He is a good boy.", "He created the robot and broke it after making it."]
            print("\n\nrunning batch process", "\n", "="*20, "\n", sent, "\n",)
            print(annotator.getAnnotations(sent, batch=True, dep_parse=True))



if __name__ == "__main__":
    test(batch=True)

