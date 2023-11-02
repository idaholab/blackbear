# /****************************************************************/
# /*               DO NOT MODIFY THIS HEADER                      */
# /*                       BlackBear                              */
# /*                                                              */
# /*           (c) 2017 Battelle Energy Alliance, LLC             */
# /*                   ALL RIGHTS RESERVED                        */
# /*                                                              */
# /*          Prepared by Battelle Energy Alliance, LLC           */
# /*            Under Contract No. DE-AC07-05ID14517              */
# /*            With the U. S. Department of Energy               */
# /*                                                              */
# /*            See COPYRIGHT for full restrictions               */
# /****************************************************************/

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
