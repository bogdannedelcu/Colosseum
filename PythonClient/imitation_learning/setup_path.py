# Import this module to automatically setup path to local colosseum module
# This module first tries to see if colosseum module is installed via pip
# If it does then we don't do anything else
# Else we look up grand-parent folder to see if it has colosseum folder
#    and if it does then we add that in sys.path

import os,sys,inspect,logging

#this class simply tries to see if colosseum 
class SetupPath:
    @staticmethod
    def getDirLevels(path):
        path_norm = os.path.normpath(path)
        return len(path_norm.split(os.sep))

    @staticmethod
    def getCurrentPath():
        cur_filepath = os.path.abspath(inspect.getfile(inspect.currentframe()))
        return os.path.dirname(cur_filepath)

    @staticmethod
    def getGrandParentDir():
        cur_path = SetupPath.getCurrentPath()
        if SetupPath.getDirLevels(cur_path) >= 2:
            return os.path.dirname(os.path.dirname(cur_path))
        return ''

    @staticmethod
    def getParentDir():
        cur_path = SetupPath.getCurrentPath()
        if SetupPath.getDirLevels(cur_path) >= 1:
            return os.path.dirname(cur_path)
        return ''

    @staticmethod
    def addColosseumModulePath():
        # if colosseum module is installed then don't do anything else
        #import pkgutil
        #colosseum_loader = pkgutil.find_loader('colosseum')
        #if colosseum_loader is not None:
        #    return

        parent = SetupPath.getParentDir()
        if parent !=  '':
            colosseum_path = os.path.join(parent, 'colosseum')
            client_path = os.path.join(colosseum_path, 'client.py')
            if os.path.exists(client_path):
                sys.path.insert(0, parent)
        else:
            logging.warning("colosseum module not found in parent folder. Using installed package (pip install colosseum).")

SetupPath.addColosseumModulePath()
