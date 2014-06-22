from distutils.core import setup
from distutils.command.install import INSTALL_SCHEMES
files = ["practnlptools/*"]
import os
def walkdir(dirname):
    rec_files=[]
    for cur, _dirs, files in os.walk(dirname):
    	rec_files+=[(cur,map(lambda f: os.path.join(cur,f),files))]
    #print rec_files
    return rec_files
#walkdir("practnlp")
for scheme in INSTALL_SCHEMES.values():
    scheme['data'] = scheme['purelib']

setup(name = "practNLPTools",
    version = "1.0",
    description = "Practical Natural Language Processing for Humans.",
    author = "Biplab Ch Das",
    author_email = "biplab12@cse.iitb.ac.in",
    url = "http://www.cse.iitb.ac.in/~biplab12",
    #Name the folder where your packages live:
    #(If you have other packages (dirs) or modules (py files) then
    #put them into the package directory - they will be found 
    #recursively.)
    packages = ['practnlptools'],
    #'package' package must contain files (see list above)
    #I called the package 'package' thus cleverly confusing the whole issue...
    #This dict maps the package name =to=> directories
    #It says, package *needs* these files.
    package_data = {'practnlptools' : files },
    #'runner' is in the root.
    #scripts = ["runner.py"],
    long_description = """Practical Natural Language Processing for Humans.""",
    data_files = walkdir('practnlptools')
    #
    #This next part it for the Cheese Shop, look a little down the page.
    #classifiers = []    
)
