# * This file is part of the MOOSE framework
# * https://www.mooseframework.org
# *
# * All rights reserved, see COPYRIGHT for full restrictions
# * https://github.com/idaholab/moose/blob/master/COPYRIGHT
# *
# * Licensed under LGPL 2.1, please see LICENSE for details
# * https://www.gnu.org/licenses/lgpl-2.1.html

from MooseDocs.base import Extension
from MooseDocs.extensions import appsyntax


def make_extension(**kwargs):
    return BlackBearExtension(**kwargs)


def hasObject(ext, name):
    for ext in ext.translator.extensions:
        if isinstance(ext, appsyntax.AppSyntaxExtension):
            node = ext._object_cache.get(name, None)
            if node is not None:
                return True
            break
    return False


class BlackBearExtension(Extension):
    """
    Adds features useful for BlackBear developers.
    """
