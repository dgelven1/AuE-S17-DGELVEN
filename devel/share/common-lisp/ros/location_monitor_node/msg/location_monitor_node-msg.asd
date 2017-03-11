
(cl:in-package :asdf)

(defsystem "location_monitor_node-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "LandmarkDistance" :depends-on ("_package_LandmarkDistance"))
    (:file "_package_LandmarkDistance" :depends-on ("_package"))
  ))